/*
 * vClient.cpp — Vanguard pipe interceptor V5
 *
 * Flow:
 *   1. Stop/restart VGC, take named pipe
 *   2. Read pipe -> extract RSO JWT + ID JWT + SID + PUUID
 *   3. Fetch entitlement token from Riot entitlements service
 *   4. Connect to VPS via TLS (Schannel)
 *   5. Send SESSION_AUTH matching protocol.hpp PackSessionAuth exactly
 *   6. Receive SESSION_AUTH_OK -> extract gateway envelope
 *   7. POST envelope to Riot gateway via WinHTTP
 *
 * Build:
 *   cl vClient.cpp /EHsc /std:c++17 /O2 /W3
 *      /link winhttp.lib ws2_32.lib secur32.lib bcrypt.lib Crypt32.lib Advapi32.lib
 */

#define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <security.h>
#include <schannel.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <TlHelp32.h>
#include <sddl.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Advapi32.lib")

 // =============================================================================
 //  CONFIG
 // =============================================================================
static std::string       g_vps_host = "192.168.1.136";
#define VPS_HOST g_vps_host.c_str()
constexpr uint16_t       VPS_PORT = 51820;
constexpr const char* AUTH_KEY = "feqxYc-ilusao";
constexpr bool           TLS_SKIP_VERIFY = true;

constexpr const wchar_t* GW_REGION = L"la";
constexpr const wchar_t* GW_PATH = L"/vanguard/v1/gateway";
constexpr INTERNET_PORT  GW_PORT = 8443;
constexpr const wchar_t* VGC_UA = L"vanguard/1.18.3-74+20260623.212037";

constexpr const wchar_t* PIPE_NAME =
L"\\\\.\\pipe\\933823D3-C77B-4BAE-89D2-A92B567236BC";

constexpr uint32_t MSG_SESSION_AUTH = 14;
constexpr uint32_t MSG_SESSION_AUTH_OK = 15;
constexpr uint32_t MSG_PING = 7;
constexpr uint32_t MSG_PONG = 8;
constexpr uint32_t MSG_ERROR = 9;
// =============================================================================

static std::atomic_bool g_shutdown(false);
static std::atomic_bool g_auth_successful(false);
static std::atomic_bool g_in_progress(false);
static std::mutex       g_log_mtx;
static std::ofstream    g_log_file;
static uint32_t         g_valorant_pid = 0;

static uint32_t GetValorantPID();
static uint32_t GetRiotClientPID();

static void Log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(g_log_mtx);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm_buf{};
    localtime_s(&tm_buf, &now);
    std::ostringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " " << msg;
    std::string line = ss.str();
    std::cout << line << std::endl;
    if (g_log_file.is_open()) {
        g_log_file << line << std::endl;
        g_log_file.flush();
    }
}

// ── VGC Service Emulation (fixes VAL 5) ──────────────────────────────────────

static HANDLE g_vgc_event = nullptr;

static void CreateVgcEvent() {
    // Create the Global\AntiVgc event that Valorant checks to verify VGC is running
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    ConvertStringSecurityDescriptorToSecurityDescriptorW(
        L"D:(A;;GA;;;WD)S:(ML;;NW;;;LW)",
        SDDL_REVISION_1,
        &sa.lpSecurityDescriptor,
        nullptr
    );
    g_vgc_event = CreateEventW(&sa, TRUE, TRUE, L"Global\\AntiVgc");
    if (g_vgc_event) {
        SetEvent(g_vgc_event);
        Log("[VGC-EMU] Created Global\\AntiVgc event (signaled)");
    } else {
        Log("[VGC-EMU] Failed to create Global\\AntiVgc event err=" + std::to_string(GetLastError()));
    }
    if (sa.lpSecurityDescriptor) LocalFree(sa.lpSecurityDescriptor);
}

static void WriteSessionAuthRegistry() {
    // Write registry key that the VGC service sets when initialized
    HKEY hk = nullptr;
    LONG res = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\SessionAuth", 0, nullptr,
        REG_OPTION_VOLATILE, KEY_WRITE, nullptr, &hk, nullptr);
    if (res == ERROR_SUCCESS) {
        DWORD val = 1;
        RegSetValueExW(hk, L"Initialized", 0, REG_DWORD, (BYTE*)&val, sizeof(val));
        RegSetValueExW(hk, L"Status", 0, REG_DWORD, (BYTE*)&val, sizeof(val));
        RegCloseKey(hk);
        Log("[VGC-EMU] Wrote Software\\SessionAuth registry (Initialized=1)");
    } else {
        Log("[VGC-EMU] Registry write failed err=" + std::to_string(res));
    }
}

static void EmulateVgcService() {
    // Ensure event and registry exist without forcefully stopping real driver until game starts
    CreateVgcEvent();
    WriteSessionAuthRegistry();
    Log("[VGC-EMU] VGC service emulation prepared");
}

// ── Wire helpers ──────────────────────────────────────────────────────────────

static void PushU32BE(std::vector<uint8_t>& v, uint32_t x) {
    v.push_back((x >> 24) & 0xFF);
    v.push_back((x >> 16) & 0xFF);
    v.push_back((x >> 8) & 0xFF);
    v.push_back(x & 0xFF);
}

static void PushU64BE(std::vector<uint8_t>& v, uint64_t x) {
    for (int i = 7; i >= 0; i--) v.push_back((uint8_t)(x >> (i * 8)));
}

static void PushLenStr(std::vector<uint8_t>& v, const std::string& s) {
    PushU32BE(v, (uint32_t)s.size());
    v.insert(v.end(), s.begin(), s.end());
}

static void PushLenBytes(std::vector<uint8_t>& v, const std::vector<uint8_t>& b) {
    PushU32BE(v, (uint32_t)b.size());
    v.insert(v.end(), b.begin(), b.end());
}

static uint32_t ReadU32BE(const uint8_t* p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
        ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static std::vector<uint8_t> PackMsg(uint32_t type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> pkt;
    PushU32BE(pkt, type);
    PushU32BE(pkt, (uint32_t)payload.size());
    pkt.insert(pkt.end(), payload.begin(), payload.end());
    return pkt;
}

// ── Machine HWID ─────────────────────────────────────────────────────────────

static std::string RegReadStr(HKEY root, const wchar_t* sub, const wchar_t* val) {
    HKEY hk = nullptr;
    if (RegOpenKeyExW(root, sub, 0, KEY_READ, &hk) != ERROR_SUCCESS) return {};
    wchar_t buf[512]{}; DWORD sz = sizeof(buf);
    RegQueryValueExW(hk, val, nullptr, nullptr, (LPBYTE)buf, &sz);
    RegCloseKey(hk);
    std::string out;
    for (int i = 0; buf[i] && i < 256; i++) out += (char)(buf[i] & 0xFF);
    return out;
}

static std::vector<uint8_t> GetRealHwid() {
    std::string bios = RegReadStr(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BIOSVersion");
    if (bios.empty()) bios = RegReadStr(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"SystemProductName");

    std::string cpu = RegReadStr(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString");
    if (cpu.empty()) cpu = RegReadStr(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"Identifier");

    wchar_t sysRoot[MAX_PATH]{}; GetSystemDirectoryW(sysRoot, MAX_PATH); sysRoot[3] = L'\0';
    DWORD volSerial = 0;
    GetVolumeInformationW(sysRoot, nullptr, 0, &volSerial, nullptr, nullptr, nullptr, 0);
    char volBuf[16]; sprintf_s(volBuf, "%08X", volSerial);

    std::string guid = RegReadStr(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid");

    std::string composite = "BIOS:" + bios + "|CPU:" + cpu + "|VOL:" +
        std::string(volBuf) + "|MGUID:" + guid;
    Log("[HWID] composite: " + composite.substr(0, 80) + "...");

    std::vector<uint8_t> hash(32, 0);
    HCRYPTPROV hProv = 0; HCRYPTHASH hHash = 0; DWORD hashLen = 32;
    CryptAcquireContextW(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);
    CryptHashData(hHash, (BYTE*)composite.data(), (DWORD)composite.size(), 0);
    CryptGetHashParam(hHash, HP_HASHVAL, hash.data(), &hashLen, 0);
    CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0);

    std::ostringstream hex;
    for (auto b : hash) hex << std::hex << std::setfill('0') << std::setw(2) << (int)b;
    Log("[HWID] sha256=" + hex.str());
    return hash;
}

// ── CPU / GPU info ────────────────────────────────────────────────────────────

static void GetCpuInfo(std::string& brand, std::string& model, uint32_t& cores) {
    model = RegReadStr(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString");
    while (!model.empty() && (model.back() == ' ' || model.back() == '\t')) model.pop_back();
    if (model.find("Intel") != std::string::npos) brand = "Intel";
    else if (model.find("AMD") != std::string::npos) brand = "AMD";
    else brand = "Unknown";
    SYSTEM_INFO si{}; GetSystemInfo(&si);
    cores = si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 1;
}

static void GetGpuInfo(std::string& brand, std::string& model) {
    model = RegReadStr(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\Class\\"
        L"{4d36e968-e325-11ce-bfc1-08002be10318}\\0000", L"DriverDesc");
    if (model.empty()) { brand = "Unknown"; model = "Unknown"; return; }
    if (model.find("NVIDIA") != std::string::npos) brand = "NVIDIA";
    else if (model.find("AMD") != std::string::npos ||
        model.find("Radeon") != std::string::npos) brand = "AMD";
    else if (model.find("Intel") != std::string::npos) brand = "Intel";
    else brand = "Unknown";
}

// ── RSA-2048 keypair → PEM SPKI ───────────────────────────────────────────────

static std::string Base64Encode(const uint8_t* data, size_t len) {
    static const char* tbl =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (size_t i = 0; i < len; i++) {
        val = (val << 8) + data[i]; valb += 8;
        while (valb >= 0) { out += tbl[(val >> valb) & 0x3F]; valb -= 6; }
    }
    if (valb > -6) out += tbl[((val << 8) >> (valb + 8)) & 0x3F];
    while (out.size() % 4) out += '=';
    return out;
}

static std::vector<uint8_t> BcryptBlobToSpkiDer(const std::vector<uint8_t>& pubBlob) {
    auto* blob = (BCRYPT_RSAKEY_BLOB*)pubBlob.data();
    DWORD expLen = blob->cbPublicExp;
    DWORD modLen = blob->cbModulus;
    const uint8_t* expBytes = pubBlob.data() + sizeof(BCRYPT_RSAKEY_BLOB);
    const uint8_t* modBytes = expBytes + expLen;

    auto der_len = [](size_t len, std::vector<uint8_t>& buf) {
        if (len < 0x80) { buf.push_back((uint8_t)len); }
        else if (len < 0x100) { buf.push_back(0x81); buf.push_back((uint8_t)len); }
        else { buf.push_back(0x82); buf.push_back((uint8_t)(len >> 8)); buf.push_back((uint8_t)len); }
        };

    auto der_int = [&der_len](const uint8_t* d, size_t sz) -> std::vector<uint8_t> {
        std::vector<uint8_t> r;
        r.push_back(0x02);
        size_t skip = 0;
        while (skip + 1 < sz && d[skip] == 0) skip++;
        bool pad = (d[skip] & 0x80) != 0;
        der_len(sz - skip + (pad ? 1 : 0), r);
        if (pad) r.push_back(0x00);
        r.insert(r.end(), d + skip, d + sz);
        return r;
        };

    auto mod_int = der_int(modBytes, modLen);
    auto exp_int = der_int(expBytes, expLen);

    std::vector<uint8_t> rsa_pk;
    rsa_pk.push_back(0x30);
    std::vector<uint8_t> rsa_pk_body;
    rsa_pk_body.insert(rsa_pk_body.end(), mod_int.begin(), mod_int.end());
    rsa_pk_body.insert(rsa_pk_body.end(), exp_int.begin(), exp_int.end());
    der_len(rsa_pk_body.size(), rsa_pk);
    rsa_pk.insert(rsa_pk.end(), rsa_pk_body.begin(), rsa_pk_body.end());

    std::vector<uint8_t> bit_str;
    bit_str.push_back(0x03);
    der_len(rsa_pk.size() + 1, bit_str);
    bit_str.push_back(0x00);
    bit_str.insert(bit_str.end(), rsa_pk.begin(), rsa_pk.end());

    static const uint8_t alg_oid[] = {
        0x30, 0x0D, 0x06, 0x09,
        0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01,
        0x05, 0x00
    };

    std::vector<uint8_t> spki_body;
    spki_body.insert(spki_body.end(), alg_oid, alg_oid + sizeof(alg_oid));
    spki_body.insert(spki_body.end(), bit_str.begin(), bit_str.end());

    std::vector<uint8_t> der_spki;
    der_spki.push_back(0x30);
    der_len(spki_body.size(), der_spki);
    der_spki.insert(der_spki.end(), spki_body.begin(), spki_body.end());
    return der_spki;
}

static std::vector<uint8_t> GenerateRsaSpkiPem() {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;
    std::vector<uint8_t> result;

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RSA_ALGORITHM, nullptr, 0) != 0) {
        Log("[RSA] BCryptOpenAlgorithmProvider failed"); return result;
    }
    if (BCryptGenerateKeyPair(hAlg, &hKey, 2048, 0) != 0) {
        Log("[RSA] BCryptGenerateKeyPair failed");
        BCryptCloseAlgorithmProvider(hAlg, 0); return result;
    }
    if (BCryptFinalizeKeyPair(hKey, 0) != 0) {
        Log("[RSA] BCryptFinalizeKeyPair failed");
        BCryptDestroyKey(hKey); BCryptCloseAlgorithmProvider(hAlg, 0); return result;
    }

    DWORD pubSz = 0;
    BCryptExportKey(hKey, nullptr, BCRYPT_RSAPUBLIC_BLOB, nullptr, 0, &pubSz, 0);
    std::vector<uint8_t> pubBlob(pubSz);
    if (BCryptExportKey(hKey, nullptr, BCRYPT_RSAPUBLIC_BLOB,
        pubBlob.data(), pubSz, &pubSz, 0) != 0) {
        Log("[RSA] BCryptExportKey failed");
        BCryptDestroyKey(hKey); BCryptCloseAlgorithmProvider(hAlg, 0); return result;
    }
    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    auto der = BcryptBlobToSpkiDer(pubBlob);
    if (der.empty()) { Log("[RSA] DER build failed"); return result; }

    std::string b64 = Base64Encode(der.data(), der.size());
    std::string pem = "-----BEGIN PUBLIC KEY-----\n";
    for (size_t i = 0; i < b64.size(); i += 64)
        pem += b64.substr(i, 64) + "\n";
    pem += "-----END PUBLIC KEY-----\n";

    result.assign(pem.begin(), pem.end());
    Log("[RSA] PEM SPKI generated " + std::to_string(result.size()) + "B");
    return result;
}

// ── Entitlement token fetch ───────────────────────────────────────────────────

static std::string FetchEntitlementsToken(const std::string& rso_jwt) {
    HINTERNET hS = WinHttpOpen(
        L"RiotGamesApi/26.3.5.0 entitlements (Windows;10;;Professional, x64) valorant/13.00.00.4990475",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hS) { Log("[ENT] WinHttpOpen failed"); return ""; }

    HINTERNET hC = WinHttpConnect(hS, L"entitlements.auth.riotgames.com", 443, 0);
    if (!hC) { WinHttpCloseHandle(hS); Log("[ENT] Connect failed"); return ""; }

    HINTERNET hR = WinHttpOpenRequest(hC, L"POST", L"/api/token/v1",
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hR) { WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return ""; }

    std::wstring headers;
    headers += L"Content-Type: application/json\r\n";
    headers += L"Accept: application/json\r\n";
    if (!rso_jwt.empty()) {
        std::wstring wjwt(rso_jwt.begin(), rso_jwt.end());
        headers += L"Authorization: Bearer " + wjwt + L"\r\n";
    }

    const char* empty_json = "{}";
    BOOL ok = WinHttpSendRequest(hR, headers.c_str(), (DWORD)-1L,
        (LPVOID)empty_json, (DWORD)strlen(empty_json), (DWORD)strlen(empty_json), 0);
    if (!ok || !WinHttpReceiveResponse(hR, nullptr)) {
        Log("[ENT] Send/recv failed err=" + std::to_string(GetLastError()));
        WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);
        return "";
    }

    DWORD status = 0, sz = sizeof(DWORD);
    WinHttpQueryHeaders(hR, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &sz, WINHTTP_NO_HEADER_INDEX);

    std::string resp_body;
    DWORD avail = 0;
    while (WinHttpQueryDataAvailable(hR, &avail) && avail > 0) {
        std::vector<char> chunk(avail); DWORD rd = 0;
        WinHttpReadData(hR, chunk.data(), avail, &rd);
        resp_body.append(chunk.data(), rd);
    }
    WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);

    Log("[ENT] HTTP " + std::to_string(status) + " body=" + resp_body.substr(0, 100));

    static const std::regex jwt_re(
        R"((eyJ[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}))");
    std::smatch m;
    if (std::regex_search(resp_body, m, jwt_re)) {
        Log("[ENT] Got entitlements_token len=" + std::to_string(m[1].str().size()));
        return m[1].str();
    }
    return "";
}

static std::string FetchIdJwt(const std::string& rso_jwt) {
    HINTERNET hS = WinHttpOpen(
        L"RiotGamesApi/26.3.5.0 entitlements (Windows;10;;Professional, x64) valorant/13.00.00.4990475",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hS) { Log("[IDT] WinHttpOpen failed"); return ""; }

    HINTERNET hC = WinHttpConnect(hS, L"auth.riotgames.com", 443, 0);
    if (!hC) { WinHttpCloseHandle(hS); Log("[IDT] Connect failed"); return ""; }

    HINTERNET hR = WinHttpOpenRequest(hC, L"GET", L"/userinfo",
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hR) { WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return ""; }

    std::wstring headers;
    headers += L"Accept: application/json\r\n";
    if (!rso_jwt.empty()) {
        std::wstring wjwt(rso_jwt.begin(), rso_jwt.end());
        headers += L"Authorization: Bearer " + wjwt + L"\r\n";
    }

    BOOL ok = WinHttpSendRequest(hR, headers.c_str(), (DWORD)-1L,
        nullptr, 0, 0, 0);
    if (!ok || !WinHttpReceiveResponse(hR, nullptr)) {
        Log("[IDT] Send/recv failed err=" + std::to_string(GetLastError()));
        WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);
        return "";
    }

    DWORD status = 0, sz = sizeof(DWORD);
    WinHttpQueryHeaders(hR, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &sz, WINHTTP_NO_HEADER_INDEX);

    std::string resp_body;
    DWORD avail = 0;
    while (WinHttpQueryDataAvailable(hR, &avail) && avail > 0) {
        std::vector<char> chunk(avail); DWORD rd = 0;
        WinHttpReadData(hR, chunk.data(), avail, &rd);
        resp_body.append(chunk.data(), rd);
    }
    WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);

    Log("[IDT] HTTP " + std::to_string(status) + " body=" + resp_body.substr(0, 100));

    static const std::regex jwt_re(
        R"((eyJ[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}))");
    std::smatch m;
    if (std::regex_search(resp_body, m, jwt_re)) {
        Log("[IDT] Got id_token len=" + std::to_string(m[1].str().size()));
        return m[1].str();
    }
    Log("[IDT] No id_token found in response");
    return "";
}

// ── SESSION_AUTH payload ──────────────────────────────────────────────────────

static std::vector<uint8_t> BuildSessionAuth(
    const std::string& jwt,
    const std::string& puuid,
    const std::string& external_sid,
    const std::string& region,
    uint32_t           pid,
    const std::vector<uint8_t>& hwid,
    const std::vector<uint8_t>& rsa_spki_pem,
    const std::string& cpu_brand,
    const std::string& cpu_model,
    const std::string& gpu_brand,
    const std::string& gpu_model,
    uint32_t           cpu_logical_count)
{
    std::vector<uint8_t> body;

    PushLenStr(body, AUTH_KEY);
    PushLenBytes(body, hwid);
    PushLenStr(body, jwt);
    PushLenStr(body, puuid);
    PushU32BE(body, pid);

    uint64_t now_ms = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    PushU64BE(body, now_ms);

    PushLenStr(body, region);
    PushLenBytes(body, hwid);
    PushLenStr(body, puuid);

    wchar_t hn[MAX_COMPUTERNAME_LENGTH + 1]{}; DWORD hnsz = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameW(hn, &hnsz);
    std::string hostname;
    for (int i = 0; hn[i]; i++) hostname += (char)(hn[i] & 0xFF);
    PushLenStr(body, hostname.empty() ? "WIN-PC" : hostname);

    PushLenBytes(body, rsa_spki_pem);

    PushLenStr(body, "release-13.00-shipping-30-4955671");
    PushU32BE(body, 4955671);
    PushU32BE(body, 13);
    PushU32BE(body, 0);
    PushU32BE(body, 30);
    PushU32BE(body, 0);

    PushLenStr(body, external_sid);

    PushLenStr(body, cpu_brand);
    PushLenStr(body, cpu_model);
    PushLenStr(body, gpu_brand);
    PushLenStr(body, gpu_model);
    PushU32BE(body, cpu_logical_count);

    return body;
}

// ── Parse SESSION_AUTH_OK ─────────────────────────────────────────────────────

static std::vector<uint8_t> ParseSessionAuthOk(const std::vector<uint8_t>& payload) {
    if (payload.size() < 4) return {};
    uint32_t sid_len = ReadU32BE(payload.data());
    uint32_t gw_off = 4 + sid_len;
    if (gw_off + 4 > payload.size()) return {};
    uint32_t gw_len = ReadU32BE(payload.data() + gw_off);
    if (gw_off + 4 + gw_len > payload.size()) return {};
    std::string sid(payload.begin() + 4, payload.begin() + 4 + sid_len);
    Log("[VPS] session_id=" + sid.substr(0, 8) + "... gw_body=" + std::to_string(gw_len) + "B");
    return std::vector<uint8_t>(payload.begin() + gw_off + 4,
        payload.begin() + gw_off + 4 + gw_len);
}

// ── Schannel TLS client ───────────────────────────────────────────────────────

struct SspiHandle {
    CredHandle cred{}; CtxtHandle ctx{};
    bool cred_ok = false; bool ctx_ok = false;
    ~SspiHandle() {
        if (ctx_ok)  DeleteSecurityContext(&ctx);
        if (cred_ok) FreeCredentialsHandle(&cred);
    }
};

class TlsSocket {
public:
    SOCKET s = INVALID_SOCKET;
    SspiHandle* ss = nullptr;
    std::vector<uint8_t> enc_pending, plain_pending;

    bool Connect(const char* host, uint16_t port, bool skip_verify) {
        WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) return false;
        sockaddr_in addr{};
        addr.sin_family = AF_INET; addr.sin_port = htons(port);
        InetPtonA(AF_INET, host, &addr.sin_addr);
        if (connect(s, (sockaddr*)&addr, sizeof(addr)) != 0) {
            closesocket(s); s = INVALID_SOCKET; return false;
        }
        const DWORD t = 60000;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof(t));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&t, sizeof(t));
        ss = new SspiHandle();
        if (!Handshake(host, skip_verify)) {
            delete ss; ss = nullptr; closesocket(s); s = INVALID_SOCKET; return false;
        }
        return true;
    }

    bool Handshake(const char* host, bool skip_verify) {
        SCHANNEL_CRED sc{};
        sc.dwVersion = SCHANNEL_CRED_VERSION;
        sc.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
        if (skip_verify) sc.dwFlags = SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION;
        TimeStamp ts{};
        if (AcquireCredentialsHandleW(nullptr, const_cast<SEC_WCHAR*>(UNISP_NAME_W),
            SECPKG_CRED_OUTBOUND, nullptr, &sc, nullptr, nullptr,
            &ss->cred, &ts) != SEC_E_OK) return false;
        ss->cred_ok = true;
        std::vector<uint8_t> inbuf(32 * 1024), outbuf(32 * 1024);
        SecBufferDesc in_desc{}; SecBuffer in_sec[2]{};
        DWORD ctx_attr = 0; bool first = true;
        const std::wstring whost(host, host + strlen(host));
        for (;;) {
            SecBuffer out_sec[1]{};
            SecBufferDesc out_desc{ SECBUFFER_VERSION, 1, out_sec };
            out_sec[0].BufferType = SECBUFFER_TOKEN;
            out_sec[0].pvBuffer = outbuf.data();
            out_sec[0].cbBuffer = (ULONG)outbuf.size();
            SECURITY_STATUS st = InitializeSecurityContextW(
                &ss->cred, first ? nullptr : &ss->ctx,
                const_cast<wchar_t*>(whost.c_str()),
                ISC_REQ_SEQUENCE_DETECT | ISC_REQ_CONFIDENTIALITY |
                ISC_REQ_STREAM | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_USE_SUPPLIED_CREDS,
                0, SECURITY_NATIVE_DREP, first ? nullptr : &in_desc, 0,
                &ss->ctx, &out_desc, &ctx_attr, &ts);
            first = false;
            if (st != SEC_E_OK && st != SEC_I_CONTINUE_NEEDED) return false;
            ss->ctx_ok = true;
            if (out_sec[0].cbBuffer && out_sec[0].pvBuffer) {
                send(s, (const char*)out_sec[0].pvBuffer, out_sec[0].cbBuffer, 0);
                FreeContextBuffer(out_sec[0].pvBuffer);
            }
            if (st == SEC_E_OK) return true;
            int got = recv(s, (char*)inbuf.data(), (int)inbuf.size(), 0);
            if (got <= 0) return false;
            in_sec[0] = { (ULONG)got, SECBUFFER_TOKEN, inbuf.data() };
            in_sec[1] = { 0, SECBUFFER_EMPTY, nullptr };
            in_desc = { SECBUFFER_VERSION, 2, in_sec };
        }
    }

    void SendAll(const uint8_t* data, size_t len) {
        SecPkgContext_StreamSizes sizes{};
        QueryContextAttributesW(&ss->ctx, SECPKG_ATTR_STREAM_SIZES, &sizes);
        size_t max_chunk = sizes.cbMaximumMessage > 0 ? sizes.cbMaximumMessage : len;
        size_t off = 0;
        while (off < len) {
            size_t chunk = (std::min)(len - off, max_chunk);
            std::vector<uint8_t> buf(sizes.cbHeader + chunk + sizes.cbTrailer);
            memcpy(buf.data() + sizes.cbHeader, data + off, chunk);
            SecBuffer sec[4]{};
            sec[0] = { sizes.cbHeader,  SECBUFFER_STREAM_HEADER,  buf.data() };
            sec[1] = { (ULONG)chunk,    SECBUFFER_DATA,           buf.data() + sizes.cbHeader };
            sec[2] = { sizes.cbTrailer, SECBUFFER_STREAM_TRAILER, buf.data() + sizes.cbHeader + chunk };
            sec[3] = { 0, SECBUFFER_EMPTY, nullptr };
            SecBufferDesc desc{ SECBUFFER_VERSION, 4, sec };
            EncryptMessage(&ss->ctx, 0, &desc, 0);
            ULONG total = sec[0].cbBuffer + sec[1].cbBuffer + sec[2].cbBuffer;
            send(s, (const char*)buf.data(), total, 0);
            off += chunk;
        }
    }

    void Drain() {
        while (!enc_pending.empty()) {
            SecBuffer sec[4]{};
            sec[0] = { (ULONG)enc_pending.size(), SECBUFFER_DATA, enc_pending.data() };
            for (int i = 1; i < 4; i++) sec[i].BufferType = SECBUFFER_EMPTY;
            SecBufferDesc desc{ SECBUFFER_VERSION, 4, sec };
            SECURITY_STATUS st = DecryptMessage(&ss->ctx, &desc, 0, nullptr);
            if (st == SEC_E_INCOMPLETE_MESSAGE) break;
            if (st != SEC_E_OK) throw std::runtime_error("TLS decrypt failed");
            size_t extra_off = enc_pending.size(), extra_len = 0;
            for (int i = 0; i < 4; i++) {
                if (sec[i].BufferType == SECBUFFER_DATA && sec[i].cbBuffer)
                    plain_pending.insert(plain_pending.end(),
                        (uint8_t*)sec[i].pvBuffer,
                        (uint8_t*)sec[i].pvBuffer + sec[i].cbBuffer);
                if (sec[i].BufferType == SECBUFFER_EXTRA && sec[i].cbBuffer) {
                    extra_off = (uint8_t*)sec[i].pvBuffer - enc_pending.data();
                    extra_len = sec[i].cbBuffer;
                }
            }
            if (extra_len)
                enc_pending.assign(enc_pending.begin() + extra_off,
                    enc_pending.begin() + extra_off + extra_len);
            else enc_pending.clear();
        }
    }

    std::vector<uint8_t> RecvMsg() {
        for (;;) {
            Drain();
            if (plain_pending.size() >= 8) {
                uint32_t plen = ReadU32BE(plain_pending.data() + 4);
                size_t need = 8 + plen;
                if (plain_pending.size() >= need) {
                    std::vector<uint8_t> msg(plain_pending.begin(),
                        plain_pending.begin() + need);
                    plain_pending.erase(plain_pending.begin(),
                        plain_pending.begin() + need);
                    return msg;
                }
            }
            uint8_t chunk[16 * 1024];
            int got = recv(s, (char*)chunk, sizeof(chunk), 0);
            if (got <= 0) throw std::runtime_error("recv closed");
            enc_pending.insert(enc_pending.end(), chunk, chunk + got);
        }
    }

    void Close() {
        if (ss) { delete ss; ss = nullptr; }
        if (s != INVALID_SOCKET) { closesocket(s); s = INVALID_SOCKET; }
    }
};

// ── Gateway POST ──────────────────────────────────────────────────────────────

static const uint8_t FALLBACK_TOKEN_RAW[293] = {
    0x08, 0x01, 0x12, 0xA0, 0x02, 0x52, 0x47, 0x01, 0x00, 0x05, 0xFA, 0xA7,
    0x74, 0xC9, 0x93, 0x69, 0x50, 0x77, 0xF4, 0xB0, 0xD9, 0xC8, 0x0D, 0x6F,
    0x67, 0x57, 0x08, 0xCB, 0xFC, 0x03, 0x06, 0x60, 0x70, 0x2C, 0x73, 0x9E,
    0x2C, 0xA5, 0xF7, 0x25, 0xF0, 0x4E, 0x2A, 0x8F, 0x9F, 0xB5, 0xC7, 0x06,
    0xA9, 0x4E, 0x78, 0x15, 0x7B, 0x20, 0x7D, 0xD3, 0x0F, 0xC5, 0xB8, 0x24,
    0xEE, 0xD2, 0xBC, 0xA1, 0x9E, 0x83, 0x0F, 0x34, 0x98, 0x2F, 0x3D, 0xED,
    0xF1, 0x3A, 0xD2, 0x63, 0xDC, 0xA0, 0xA6, 0x16, 0x9F, 0xAA, 0x21, 0xD5,
    0xA4, 0xE9, 0x1C, 0xFE, 0xB6, 0x7A, 0xC2, 0x4B, 0x0C, 0x6F, 0x90, 0x7B,
    0x6F, 0x80, 0x77, 0x70, 0x67, 0x3B, 0x0A, 0xB5, 0x2A, 0x4A, 0x71, 0xBF,
    0xBE, 0xE9, 0xBE, 0x4C, 0xBE, 0xF3, 0xC2, 0xBE, 0xCD, 0x2F, 0xB2, 0xDA,
    0xE8, 0x82, 0xDB, 0xDD, 0x3F, 0xF0, 0x5A, 0x98, 0x0D, 0xA0, 0x2D, 0x7F,
    0xAD, 0xDA, 0xE7, 0xD6, 0xF5, 0x9D, 0x32, 0x1D, 0x0B, 0x38, 0x48, 0x9F,
    0x03, 0xBD, 0x23, 0xF0, 0x39, 0x76, 0x52, 0x67, 0x8F, 0x02, 0x32, 0x3B,
    0xBC, 0x82, 0xCA, 0x10, 0xDE, 0x6A, 0xC7, 0x3C, 0x51, 0x14, 0xFF, 0x58,
    0x8B, 0xFE, 0x7B, 0x63, 0xA6, 0xE2, 0x9D, 0xDB, 0x5B, 0xC0, 0xCD, 0x7F,
    0x92, 0xCE, 0xA6, 0x5D, 0x0C, 0x19, 0x25, 0x00, 0x6E, 0xDC, 0x7B, 0x3B,
    0x0F, 0x68, 0x2B, 0xE1, 0xDD, 0xE8, 0x66, 0x03, 0x70, 0x58, 0x3E, 0x5F,
    0xEA, 0xB1, 0x65, 0x68, 0x4C, 0xB1, 0x2D, 0xF9, 0x7E, 0xD9, 0x45, 0xBF,
    0x06, 0xAD, 0xDF, 0x74, 0xFC, 0x1A, 0x5F, 0x09, 0x41, 0x33, 0xA6, 0x30,
    0xF2, 0xD6, 0x02, 0xE6, 0xCB, 0x46, 0x37, 0xF3, 0x2B, 0x7A, 0xB9, 0x7A,
    0xC6, 0x06, 0x13, 0x7C, 0x0A, 0xF5, 0x78, 0xB4, 0x36, 0x43, 0xDD, 0x6E,
    0xBF, 0x68, 0xBF, 0x90, 0xC7, 0x0E, 0x7D, 0x19, 0x72, 0xBB, 0xDA, 0x9F,
    0xF5, 0x44, 0x82, 0x96, 0x2F, 0xD0, 0x2F, 0xEB, 0x49, 0xBE, 0x8B, 0x17,
    0x05, 0x5D, 0xE3, 0x8C, 0x10, 0xBA, 0xB3, 0x42, 0x7C, 0x01, 0xDD, 0xA9,
    0x00, 0xE5, 0xC2, 0x6D, 0xD0
};

static bool PostToGatewaySingle(const wchar_t* gw_host,
    const std::vector<uint8_t>& envelope,
    const std::string& puuid,
    const std::string& auth_bearer,
    const std::string& entitlements_jwt,
    const std::string& id_jwt)
{
    HINTERNET hS = WinHttpOpen(VGC_UA, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hS) { Log("[GW] WinHttpOpen failed"); return false; }
    HINTERNET hC = WinHttpConnect(hS, gw_host, GW_PORT, 0);
    if (!hC) { WinHttpCloseHandle(hS); Log("[GW] Connect failed"); return false; }
    HINTERNET hR = WinHttpOpenRequest(hC, L"POST", GW_PATH, L"HTTP/1.1",
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hR) { WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return false; }

    DWORD ssl = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
        | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
    WinHttpSetOption(hR, WINHTTP_OPTION_SECURITY_FLAGS, &ssl, sizeof(ssl));

    std::wstring headers;
    headers += L"Connection: Keep-Alive\r\n";
    headers += L"Content-Type: application/x-protobuf\r\n";
    headers += L"Accept: */*\r\n";
    headers += L"Expect: \r\n";

    if (!auth_bearer.empty()) {
        std::wstring wa(auth_bearer.begin(), auth_bearer.end());
        headers += L"Authorization: Bearer " + wa + L"\r\n";
    }
    if (!entitlements_jwt.empty()) {
        std::wstring we(entitlements_jwt.begin(), entitlements_jwt.end());
        headers += L"X-Riot-Entitlements-JWT: " + we + L"\r\n";
    }
    if (!id_jwt.empty()) {
        std::wstring wid(id_jwt.begin(), id_jwt.end());
        headers += L"X-Riot-Id-JWT: " + wid + L"\r\n";
    }
    if (!puuid.empty()) {
        std::wstring wp(puuid.begin(), puuid.end());
        headers += L"X-VG-2: " + wp + L"\r\n";
    }
    headers += L"X-VG-1: 3\r\n";
    headers += L"X-VG-3: 1\r\n";

    std::string gw_host_s(gw_host, gw_host + wcslen(gw_host));
    Log("[GW] POST " + gw_host_s + " body=" + std::to_string(envelope.size()) +
        "B puuid=" + (puuid.size() >= 8 ? puuid.substr(0, 8) : puuid));

    BOOL ok = WinHttpSendRequest(hR, headers.c_str(), (DWORD)-1L,
        (LPVOID)envelope.data(), (DWORD)envelope.size(), (DWORD)envelope.size(), 0);
    if (!ok || !WinHttpReceiveResponse(hR, nullptr)) {
        Log("[GW] Send/recv failed err=" + std::to_string(GetLastError()));
        WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return false;
    }

    DWORD status = 0, sz = sizeof(DWORD);
    WinHttpQueryHeaders(hR, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &sz, WINHTTP_NO_HEADER_INDEX);

    std::vector<uint8_t> body;
    DWORD avail = 0;
    while (WinHttpQueryDataAvailable(hR, &avail) && avail > 0) {
        std::vector<uint8_t> chunk(avail); DWORD rd = 0;
        WinHttpReadData(hR, chunk.data(), avail, &rd); chunk.resize(rd);
        body.insert(body.end(), chunk.begin(), chunk.end());
    }

    DWORD hdr_sz = 0;
    WinHttpQueryHeaders(hR, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER, &hdr_sz, WINHTTP_NO_HEADER_INDEX);
    std::string resp_headers_str;
    if (hdr_sz > 0) {
        std::vector<wchar_t> hdr_buf(hdr_sz / sizeof(wchar_t) + 1);
        if (WinHttpQueryHeaders(hR, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
            hdr_buf.data(), &hdr_sz, WINHTTP_NO_HEADER_INDEX)) {
            std::wstring whdrs(hdr_buf.data());
            resp_headers_str = std::string(whdrs.begin(), whdrs.end());
        }
    }
    WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);

    if (status == 200) {
        g_auth_successful.store(true);
        Log("[GW] *** HTTP 200 SESSION ESTABLISHED OK *** body=" + std::to_string(body.size()) + "B");
        Log("[+] =========================================================");
        Log("[+]               >>> READY TO QUEUE (READY TO Q) <<<        ");
        Log("[+]          SESSION ESTABLISHED & AUTHORIZED BY RIOT!       ");
        Log("[+] =========================================================");
        SetConsoleTitleW(L"[+] READY TO QUEUE (READY TO Q) - VGC Emulator");
        std::cout << "\n\n";
        std::cout << "\x1b[92m=================================================================\x1b[0m\n";
        std::cout << "\x1b[92m               [+] READY TO QUEUE (READY TO Q)!                  \x1b[0m\n";
        std::cout << "\x1b[92m          SESSION ESTABLISHED & AUTHORIZED BY RIOT!              \x1b[0m\n";
        std::cout << "\x1b[92m=================================================================\x1b[0m\n\n";
        return true;
    }
    else {
        Log("[GW] HTTP " + std::to_string(status) + " from " + gw_host_s + " body=" + std::to_string(body.size()) + "B");
        if (!resp_headers_str.empty()) {
            std::istringstream hss(resp_headers_str);
            std::string hline; int hcount = 0;
            while (std::getline(hss, hline) && hcount++ < 4) {
                if (!hline.empty() && hline.back() == '\r') hline.pop_back();
                if (!hline.empty()) Log("[GW-HDR] " + hline);
            }
        }
        if (!body.empty()) {
            std::string s(body.begin(), body.end());
            Log("[GW] body: " + s.substr(0, 200));
        }
        return false;
    }
}

static bool PostToGateway(const std::vector<uint8_t>& envelope,
    const std::string& puuid,
    const std::string& rso_jwt,
    const std::string& entitlement_token,
    const std::string& id_jwt)
{
    const wchar_t* hosts[] = { L"la.vg.ac.pvp.net", L"na.vg.ac.pvp.net" };
    for (const auto* h : hosts) {
        if (PostToGatewaySingle(h, envelope, puuid, rso_jwt, entitlement_token, id_jwt)) return true;
    }
    return false;
}

static bool TryLocalLockfileAuth();

// ── VPS tunnel ────────────────────────────────────────────────────────────────

static void SendDirectAuthViaVPS(const std::string& rso_jwt,
    const std::string& entitlement_token,
    const std::string& puuid,
    const std::string& sid,
    uint32_t pid)
{
    std::string id_jwt = FetchIdJwt(rso_jwt);

    Log("[VPS] Connecting " + std::string(VPS_HOST) + ":" + std::to_string(VPS_PORT));
    TlsSocket tls;
    if (!tls.Connect(VPS_HOST, VPS_PORT, TLS_SKIP_VERIFY)) {
        Log("[VPS] TLS connect failed");
        g_in_progress.store(false);
        return;
    }
    Log("[VPS] TLS connected");

    auto hwid = GetRealHwid();

    std::string cpu_brand, cpu_model, gpu_brand, gpu_model;
    uint32_t cpu_cores = 0;
    GetCpuInfo(cpu_brand, cpu_model, cpu_cores);
    GetGpuInfo(gpu_brand, gpu_model);
    Log("[VPS] CPU: " + cpu_brand + " cores=" + std::to_string(cpu_cores));
    Log("[VPS] GPU: " + gpu_brand);

    auto rsa_pem = GenerateRsaSpkiPem();
    if (rsa_pem.empty()) {
        Log("[VPS] RSA keygen failed"); tls.Close();
        g_in_progress.store(false);
        return;
    }

    std::string region = "la    ";
    std::string auth_jwt_for_vps = entitlement_token;
    auto sa_payload = BuildSessionAuth(
        auth_jwt_for_vps,
        puuid, sid, region, pid, hwid, rsa_pem,
        cpu_brand, cpu_model, gpu_brand, gpu_model, cpu_cores);

    auto sa_pkt = PackMsg(MSG_SESSION_AUTH, sa_payload);
    Log("[VPS] Sending SESSION_AUTH entitlement_len=" + std::to_string(auth_jwt_for_vps.size()) +
        " puuid=" + puuid + " sid=" + sid);

    try {
        tls.SendAll(sa_pkt.data(), sa_pkt.size());

        auto msg = tls.RecvMsg();
        uint32_t mt = ReadU32BE(msg.data());
        uint32_t plen = ReadU32BE(msg.data() + 4);
        Log("[VPS] Received msg type=" + std::to_string(mt) + " plen=" + std::to_string(plen));

        if (mt == MSG_ERROR) {
            std::string err(msg.begin() + 8, msg.end());
            Log("[VPS] Error: " + err); tls.Close();
            g_in_progress.store(false);
            return;
        }
        if (mt != MSG_SESSION_AUTH_OK) {
            Log("[VPS] Expected SESSION_AUTH_OK(15), got " + std::to_string(mt));
            tls.Close();
            g_in_progress.store(false);
            return;
        }

        std::vector<uint8_t> payload(msg.begin() + 8, msg.end());
        auto envelope = ParseSessionAuthOk(payload);

        g_auth_successful.store(true);
        Log("[VPS] *** SESSION ESTABLISHED OK ON VPS SERVER ***");
        Log("[+] =========================================================");
        Log("[+]               >>> YOU CAN QUEUE NOW <<<                  ");
        Log("[+]   VALORANT (PID: " + std::to_string(pid) + ") AUTHORIZED & ACTIVE!   ");
        Log("[+] =========================================================");
        
        std::cout << "\n\n";
        std::cout << "\x1b[92m=================================================================\x1b[0m\n";
        std::cout << "\x1b[92m                 [+] YOU CAN QUEUE NOW (READY TO Q)!             \x1b[0m\n";
        std::cout << "\x1b[92m           VALORANT (PID: " << pid << ") AUTHORIZED & ACTIVE!              \x1b[0m\n";
        std::cout << "\x1b[92m=================================================================\x1b[0m\n\n";

        // Keep-alive background thread
        std::thread([envelope, puuid, rso_jwt, entitlement_token, id_jwt]() {
            if (!envelope.empty()) {
                PostToGateway(envelope, puuid, rso_jwt, entitlement_token, id_jwt);
            }
        }).detach();

        // 5-minute countdown loop for Next Auth Request (matching paid emulator)
        std::thread([pid]() {
            int remaining_seconds = 285; // 4:45
            while (!g_shutdown.load() && g_auth_successful.load()) {
                Sleep(1000);
                remaining_seconds--;
                int mins = remaining_seconds / 60;
                int secs = remaining_seconds % 60;
                char time_buf[16];
                sprintf_s(time_buf, "%d:%02d", mins, secs);

                std::wstring title = L"[+] YOU CAN QUEUE NOW | Next Auth: " +
                    std::wstring(time_buf, time_buf + strlen(time_buf)) +
                    L" | Valorant PID: " + std::to_wstring(pid);
                SetConsoleTitleW(title.c_str());

                if (remaining_seconds <= 0) {
                    Log("[AUTH-REFRESH] Auto-refreshing session tokens with Riot Client Local API...");
                    std::cout << "\x1b[93m[+] Auto-refreshing session auth tokens...\x1b[0m\n";
                    g_in_progress.store(false);
                    TryLocalLockfileAuth();
                    remaining_seconds = 285;
                }
            }
        }).detach();

        // VPS Ping keepalive loop
        while (!g_shutdown.load()) {
            Sleep(20000);
            try {
                auto ping_pkt = PackMsg(MSG_PING, {});
                tls.SendAll(ping_pkt.data(), ping_pkt.size());
                auto pong = tls.RecvMsg();
            } catch (...) {
                break;
            }
        }
        tls.Close();
    }
    catch (const std::exception& e) {
        Log("[VPS] Exception: " + std::string(e.what())); tls.Close();
        g_in_progress.store(false);
    }
}

static void SendViaVPS(const std::vector<std::string>& all_jwts,
    const std::string& sid, const std::string& puuid,
    uint32_t pid)
{
    std::string rso_jwt;
    std::string entitlement_token;

    // Prioritize candidates:
    // Sort so tokens starting with eyJraWQi (RSO header {"kid":...) come first, HS256 last
    std::vector<std::string> sorted_jwts = all_jwts;
    std::stable_sort(sorted_jwts.begin(), sorted_jwts.end(), [](const std::string& a, const std::string& b) {
        bool a_is_rso = a.rfind("eyJraWQi", 0) == 0;
        bool b_is_rso = b.rfind("eyJraWQi", 0) == 0;
        if (a_is_rso != b_is_rso) return a_is_rso > b_is_rso;
        bool a_is_hs = a.rfind("eyJhbGciOiJIUzI1Ni", 0) == 0;
        bool b_is_hs = b.rfind("eyJhbGciOiJIUzI1Ni", 0) == 0;
        return a_is_hs < b_is_hs;
    });

    Log("[ENT] Testing " + std::to_string(sorted_jwts.size()) + " JWT candidate(s)...");
    for (const auto& cand : sorted_jwts) {
        std::string ent = FetchEntitlementsToken(cand);
        if (!ent.empty()) {
            rso_jwt = cand;
            entitlement_token = ent;
            Log("[ENT] Matched valid RSO token! Entitlement token len=" + std::to_string(ent.size()));
            break;
        }
    }

    if (rso_jwt.empty()) {
        Log("[-] No valid RSO token in this batch, continuing background scan...");
        g_in_progress.store(false);
        return;
    }

    SendDirectAuthViaVPS(rso_jwt, entitlement_token, puuid, sid, pid);
}

// ── Instant Lockfile Auth ─────────────────────────────────────────────────────

static bool TryLocalLockfileAuth() {
    if (g_auth_successful.load() || g_in_progress.load()) return false;

    // Only proceed if VALORANT (VALORANT-Win64-Shipping.exe) is actually running!
    uint32_t pid = g_valorant_pid ? g_valorant_pid : GetValorantPID();
    if (!pid) return false;
    g_valorant_pid = pid;

    char localAppData[MAX_PATH];
    if (GetEnvironmentVariableA("LOCALAPPDATA", localAppData, MAX_PATH) == 0) return false;
    std::string lockPath = std::string(localAppData) + "\\Riot Games\\Riot Client\\Config\\lockfile";

    std::ifstream lf(lockPath);
    if (!lf.is_open()) return false;

    std::string content;
    std::getline(lf, content);
    lf.close();
    if (content.empty()) return false;

    // format: name:pid:port:password:protocol
    std::vector<std::string> parts;
    std::stringstream ss(content);
    std::string item;
    while (std::getline(ss, item, ':')) {
        parts.push_back(item);
    }
    if (parts.size() < 5) return false;

    int port = 0;
    try { port = std::stoi(parts[2]); } catch (...) { return false; }
    if (port <= 0) return false;

    std::string pass = parts[3];
    std::string creds = "riot:" + pass;
    std::string b64Creds = Base64Encode((const uint8_t*)creds.data(), creds.size());

    HINTERNET hS = WinHttpOpen(
        L"RiotClient/26.3.5.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hS) return false;

    HINTERNET hC = WinHttpConnect(hS, L"127.0.0.1", (INTERNET_PORT)port, 0);
    if (!hC) { WinHttpCloseHandle(hS); return false; }

    HINTERNET hR = WinHttpOpenRequest(hC, L"GET", L"/entitlements/v1/token",
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hR) { WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return false; }

    DWORD secFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                     SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                     SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                     SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
    WinHttpSetOption(hR, WINHTTP_OPTION_SECURITY_FLAGS, &secFlags, sizeof(secFlags));

    std::wstring headers = L"Authorization: Basic " + std::wstring(b64Creds.begin(), b64Creds.end()) + L"\r\nAccept: application/json\r\n";

    if (!WinHttpSendRequest(hR, headers.c_str(), (DWORD)-1L, nullptr, 0, 0, 0) ||
        !WinHttpReceiveResponse(hR, nullptr)) {
        WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);
        return false;
    }

    DWORD status = 0, sz = sizeof(DWORD);
    WinHttpQueryHeaders(hR, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &sz, WINHTTP_NO_HEADER_INDEX);

    std::string resp_body;
    DWORD avail = 0;
    while (WinHttpQueryDataAvailable(hR, &avail) && avail > 0) {
        std::vector<char> chunk(avail); DWORD rd = 0;
        WinHttpReadData(hR, chunk.data(), avail, &rd);
        resp_body.append(chunk.data(), rd);
    }
    WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);

    if (status != 200) return false;

    static const std::regex at_re(R"RAW("accessToken"\s*:\s*"([^"]+)")RAW");
    static const std::regex tok_re(R"RAW("token"\s*:\s*"([^"]+)")RAW");
    static const std::regex sub_re(R"RAW("subject"\s*:\s*"([^"]+)")RAW");

    std::smatch m_at, m_tok, m_sub;
    if (!std::regex_search(resp_body, m_at, at_re) ||
        !std::regex_search(resp_body, m_tok, tok_re) ||
        !std::regex_search(resp_body, m_sub, sub_re)) {
        return false;
    }

    std::string rso_jwt = m_at[1].str();
    std::string entitlement_token = m_tok[1].str();
    std::string puuid = m_sub[1].str();
    std::string sid = "00000000-0000-0000-0000-000000000000";

    if (g_in_progress.exchange(true)) return false;

    Log("[FAST-AUTH] >> Instant tokens obtained from Riot Client Local API! <<");
    Log("[FAST-AUTH] PUUID: " + puuid);
    Log("[FAST-AUTH] RSO JWT len=" + std::to_string(rso_jwt.size()));
    Log("[FAST-AUTH] Entitlement token len=" + std::to_string(entitlement_token.size()));

    std::thread([rso_jwt, entitlement_token, puuid, sid, pid]() {
        SendDirectAuthViaVPS(rso_jwt, entitlement_token, puuid, sid, pid);
    }).detach();

    return true;
}

// ── Pipe extraction ───────────────────────────────────────────────────────────

static void TryExtractAndSend(const uint8_t* buf, DWORD len, bool is_from_pipe = false) {
    if (g_auth_successful.load() || g_in_progress.load()) return;

    // 1. Extract ASCII / UTF-8
    std::string ascii8(len, ' ');
    for (DWORD i = 0; i < len; i++) {
        if (buf[i] >= 0x20 && buf[i] < 0x7F) ascii8[i] = (char)buf[i];
    }

    // 2. Extract UTF-16LE (2 bytes per char: b0, 0x00)
    std::string ascii16;
    if (len >= 2) {
        ascii16.reserve(len / 2);
        for (DWORD i = 0; i + 1 < len; i += 2) {
            if (buf[i + 1] == 0 && buf[i] >= 0x20 && buf[i] < 0x7F)
                ascii16.push_back((char)buf[i]);
            else
                ascii16.push_back(' ');
        }
    }

    static const std::regex jwt_re(
        R"((eyJ[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}))");
    static const std::regex uuid_re(
        R"([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})");

    std::vector<std::string> all_jwts;
    for (const auto& text : { ascii8, ascii16 }) {
        std::sregex_iterator jit(text.begin(), text.end(), jwt_re), jend;
        for (; jit != jend; ++jit) {
            std::string j = (*jit)[1].str();
            if (std::find(all_jwts.begin(), all_jwts.end(), j) == all_jwts.end()) {
                all_jwts.push_back(j);
            }
        }
    }

    if (all_jwts.empty()) {
        if (is_from_pipe) Log("[PIPE] No JWT in packet len=" + std::to_string(len));
        return;
    }

    std::string first_uuid, last_uuid;
    for (const auto& text : { ascii8, ascii16 }) {
        std::sregex_iterator it(text.begin(), text.end(), uuid_re), end;
        for (; it != end; ++it) {
            if (first_uuid.empty()) first_uuid = it->str();
            last_uuid = it->str();
        }
    }
    std::string puuid = first_uuid;
    std::string sid = last_uuid;
    uint32_t pid = g_valorant_pid ? g_valorant_pid : GetValorantPID();
    if (!pid) return;
    g_valorant_pid = pid;

    if (g_in_progress.exchange(true)) return;

    Log("[AUTH] Intercepted JWT candidates! Count: " + std::to_string(all_jwts.size()));
    Log("[AUTH] PUUID: " + puuid);
    Log("[AUTH] SID:   " + sid);
    Log("[AUTH] PID:   " + std::to_string(pid));

    std::thread([all_jwts, sid, puuid, pid]() {
        SendViaVPS(all_jwts, sid, puuid, pid);
    }).detach();
}

static void HandleClient(HANDLE pipe) {
    std::vector<uint8_t> buf(65536);
    DWORD bytesRead; int hb_count = 0;
    
    // Send VGC init handshake immediately on connection (VGC service sends status on connect)
    {
        // VGC service initial status: magic=0x01, status=READY(1), version
        uint8_t init_msg[40] = {0};
        init_msg[0] = 0x01; // MSG_TYPE_INIT
        init_msg[1] = 0x01; // STATUS_READY
        init_msg[4] = 0x01; // VGC version major
        init_msg[5] = 0x12; // VGC version minor (1.18)
        init_msg[6] = 0x03; // patch
        DWORD bw = 0;
        WriteFile(pipe, init_msg, 40, &bw, nullptr);
        Log("[PIPE] Sent VGC init handshake (READY)");
    }
    
    while (!g_shutdown.load()) {
        if (!ReadFile(pipe, buf.data(), (DWORD)buf.size(), &bytesRead, nullptr)
            || bytesRead == 0) break;

        // Heartbeat (0x03 → respond with 0x04)
        if (buf[0] == 0x03) {
            std::vector<uint8_t> resp(buf.data(), buf.data() + bytesRead);
            resp[0] = 0x04; // HB_ACK
            DWORD bw = 0; WriteFile(pipe, resp.data(), (DWORD)resp.size(), &bw, nullptr);
            hb_count++;
            if (hb_count % 10 == 1) {
                char sz[64]; sprintf_s(sz, "[PIPE] HB ack #%d", hb_count); Log(sz);
            }
            continue;
        }
        
        // Status query (0x01 → respond with 0x02 STATUS_OK)
        if (buf[0] == 0x01) {
            std::vector<uint8_t> resp(buf.data(), buf.data() + bytesRead);
            resp[0] = 0x02; // STATUS_OK
            resp[1] = 0x01; // VGC_INITIALIZED
            DWORD bw = 0; WriteFile(pipe, resp.data(), (DWORD)resp.size(), &bw, nullptr);
            Log("[PIPE] Status query → STATUS_OK");
            continue;
        }
        
        // Auth/token check (0x05 → respond with 0x06 AUTH_OK)
        if (buf[0] == 0x05) {
            std::vector<uint8_t> resp(buf.data(), buf.data() + bytesRead);
            resp[0] = 0x06; // AUTH_OK
            DWORD bw = 0; WriteFile(pipe, resp.data(), (DWORD)resp.size(), &bw, nullptr);
            Log("[PIPE] Auth check → AUTH_OK");
            continue;
        }
        
        // Scan large payloads for tokens
        if (bytesRead > 100) {
            TryExtractAndSend(buf.data(), bytesRead, true);
        }
        
        // Default: echo with magic+1
        if (bytesRead >= 4) {
            uint32_t magic; memcpy(&magic, buf.data(), 4); uint32_t nm = magic + 1;
            std::vector<uint8_t> echo(buf.data(), buf.data() + bytesRead);
            memcpy(echo.data(), &nm, 4);
            DWORD bw = 0; WriteFile(pipe, echo.data(), (DWORD)echo.size(), &bw, nullptr);
        }
    }
    CloseHandle(pipe); Log("[PIPE] Client disconnected");
}

static void PipeServerInstance(const wchar_t* pipe_name) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    ConvertStringSecurityDescriptorToSecurityDescriptorW(
        L"D:(A;;GA;;;WD)S:(ML;;NW;;;LW)",
        SDDL_REVISION_1,
        &sa.lpSecurityDescriptor,
        nullptr
    );

    while (!g_shutdown.load()) {
        HANDLE pipe = CreateNamedPipeW(pipe_name, PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 1048576, 1048576, 500, sa.lpSecurityDescriptor ? &sa : nullptr);
        if (pipe == INVALID_HANDLE_VALUE) { Sleep(1000); continue; }
        std::string pName(pipe_name, pipe_name + wcslen(pipe_name));
        Log("[PIPE] Waiting for client on " + pName + "...");
        if (ConnectNamedPipe(pipe, nullptr) || GetLastError() == ERROR_PIPE_CONNECTED) {
            Log("[PIPE] Client connected on " + pName);
            std::thread(HandleClient, pipe).detach();
        }
        else { CloseHandle(pipe); }
    }
    if (sa.lpSecurityDescriptor) LocalFree(sa.lpSecurityDescriptor);
}

// ── Memory Scanner Fallback ───────────────────────────────────────────────────

static void ScanProcessMemory(uint32_t pid) {
    if (!pid || g_auth_successful.load() || g_in_progress.load()) return;
    HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProc) return;

    SYSTEM_INFO si;
    GetSystemInfo(&si);

    MEMORY_BASIC_INFORMATION mbi;
    uint8_t* p = (uint8_t*)si.lpMinimumApplicationAddress;
    std::vector<uint8_t> buffer;

    while (p < (uint8_t*)si.lpMaximumApplicationAddress && !g_auth_successful.load() && !g_shutdown.load()) {
        if (VirtualQueryEx(hProc, p, &mbi, sizeof(mbi)) == sizeof(mbi)) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_EXECUTE_READWRITE)) {
                SIZE_T bytesToRead = (std::min)(mbi.RegionSize, (SIZE_T)(2 * 1024 * 1024));
                buffer.resize(bytesToRead);
                SIZE_T bytesRead = 0;
                if (ReadProcessMemory(hProc, p, buffer.data(), bytesToRead, &bytesRead) && bytesRead > 100) {
                    TryExtractAndSend(buffer.data(), (DWORD)bytesRead);
                    if (g_auth_successful.load()) break;
                }
            }
            p += mbi.RegionSize;
        } else {
            p += 4096;
        }
    }
    CloseHandle(hProc);
}

// ── Utilities ─────────────────────────────────────────────────────────────────

static uint32_t GetProcessPIDByName(const wchar_t* procName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W pe; pe.dwSize = sizeof(pe); uint32_t pid = 0;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, procName) == 0) {
                pid = pe.th32ProcessID; break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap); return pid;
}

static uint32_t GetValorantPID() {
    return GetProcessPIDByName(L"VALORANT-Win64-Shipping.exe");
}

static uint32_t GetRiotClientPID() {
    return GetProcessPIDByName(L"RiotClientServices.exe");
}

BOOL WINAPI CtrlHandler(DWORD t) {
    if (t == CTRL_C_EVENT || t == CTRL_CLOSE_EVENT) { g_shutdown.store(true); return TRUE; }
    return FALSE;
}

// ── Main ──────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc > 1 && argv[1] != nullptr && strlen(argv[1]) > 0) {
        g_vps_host = argv[1];
    }
    SetConsoleTitleW(L"vClient V5 - Waiting for Auth");
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    char exePath[MAX_PATH]; GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string logPath(exePath);
    size_t s = logPath.find_last_of("\\/");
    if (s != std::string::npos) logPath = logPath.substr(0, s + 1) + "vClient.log";
    g_log_file.open(logPath, std::ios::out | std::ios::trunc);

    Log("vClient V5 starting");
    Log("VPS: " + std::string(VPS_HOST) + ":" + std::to_string(VPS_PORT));
    Log("Auth: " + std::string(AUTH_KEY).substr(0, 12) + "...");
    Log("Mode: auth401 (f2=os_info f4=entitlement_token f5=RSA_PEM f13=external_sid)");
    Log("Region: la1");

    // ── Step 1: Prepare environment ──
    std::cout << "\x1b[93m[1/5] Preparing environment...\x1b[0m\n";
    Log("[STEP 1/5] Preparing environment...");
    EmulateVgcService();

    // ── Step 2: Start Pipe Servers ──
    std::cout << "\x1b[93m[2/5] Establishing pipe servers...\x1b[0m\n";
    Log("[STEP 2/5] Starting Pipe Servers with permissive ACL...");
    std::thread([]() { PipeServerInstance(L"\\\\.\\pipe\\933823D3-C77B-4BAE-89D7-A92B567236BC"); }).detach();
    std::thread([]() { PipeServerInstance(L"\\\\.\\pipe\\933823D3-C77B-4BAE-89D2-A92B567236BC"); }).detach();
    std::thread([]() { PipeServerInstance(L"\\\\.\\pipe\\vgservice"); }).detach();
    std::thread([]() { PipeServerInstance(L"\\\\.\\pipe\\vgc"); }).detach();
    std::thread([]() { PipeServerInstance(L"\\\\.\\pipe\\vgk"); }).detach();
    std::thread([]() { PipeServerInstance(L"\\\\.\\pipe\\OffsetPipe"); }).detach();

    // ── Step 3: Wait for Valorant process ──
    std::cout << "\x1b[93m[3/5] Waiting for VALORANT launch...\x1b[0m\n";
    Log("[STEP 3/5] Waiting for VALORANT (VALORANT-Win64-Shipping.exe)...");
    while (!g_shutdown.load()) {
        g_valorant_pid = GetValorantPID();
        if (g_valorant_pid) {
            char msg[128]; sprintf_s(msg, "[+] Valorant detected (PID: %u)", g_valorant_pid); Log(msg);
            std::cout << "\x1b[92m[+] Valorant detected (PID: " << g_valorant_pid << ")\x1b[0m\n";
            break;
        }
        Sleep(200);
    }

    // ── Step 4: Intercept VGC pipe once game starts ──
    std::cout << "\x1b[93m[4/5] Intercepting VGC pipe...\x1b[0m\n";
    Log("[STEP 4/5] Intercepting VGC pipe...");
    // Keep service enabled to avoid VAN -81, only stop current instance if needed for pipe bind
    system("sc stop vgc >nul 2>&1");

    // ── Step 5: Auth request ──
    std::cout << "\x1b[93m[5/5] Sending auth request...\x1b[0m\n";
    Log("[STEP 5/5] Sending auth request...");

    // Fast Lockfile API monitor + memory scanner
    std::thread([]() {
        while (!g_shutdown.load() && !g_auth_successful.load()) {
            if (TryLocalLockfileAuth()) {
                Sleep(1000);
            }
            if (g_auth_successful.load()) break;

            uint32_t valPid = GetValorantPID();
            if (valPid) {
                g_valorant_pid = valPid;
                ScanProcessMemory(valPid);
            }
            if (g_auth_successful.load()) break;

            uint32_t rcPid = GetRiotClientPID();
            if (rcPid) {
                ScanProcessMemory(rcPid);
            }
            if (g_auth_successful.load()) break;

            Sleep(200);
        }
    }).detach();

    while (!g_shutdown.load()) Sleep(500);
    
    // Cleanup
    if (g_vgc_event) { CloseHandle(g_vgc_event); g_vgc_event = nullptr; }
    Log("Shutdown");
    return 0;
}