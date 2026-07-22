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
constexpr const char* VPS_HOST = "172.24.141.234";
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
constexpr uint32_t MSG_ERROR = 9;
// =============================================================================

static std::atomic_bool g_shutdown(false);
static std::atomic_bool g_api_called(false);
static std::mutex       g_log_mtx;
static std::ofstream    g_log_file;
static uint32_t         g_valorant_pid = 0;


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

    // Response is a JWT string directly or wrapped — extract it
    // userinfo returns a signed JWT (the id_token)
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
        if (AcquireCredentialsHandleW(nullptr, const_cast<wchar_t*>(UNISP_NAME),
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

static void PostToGateway(const std::vector<uint8_t>& envelope,
    const std::string& puuid,
    const std::string& entitlement_token,
    const std::string& id_jwt)
{
    wchar_t gw_host[64];
    swprintf_s(gw_host, L"%s.vg.ac.pvp.net", GW_REGION);

    HINTERNET hS = WinHttpOpen(VGC_UA, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hS) { Log("[GW] WinHttpOpen failed"); return; }
    HINTERNET hC = WinHttpConnect(hS, gw_host, GW_PORT, 0);
    if (!hC) { WinHttpCloseHandle(hS); Log("[GW] Connect failed"); return; }
    HINTERNET hR = WinHttpOpenRequest(hC, L"POST", GW_PATH, L"HTTP/1.1",
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hR) { WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return; }

    DWORD ssl = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
        | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
    WinHttpSetOption(hR, WINHTTP_OPTION_SECURITY_FLAGS, &ssl, sizeof(ssl));

    std::wstring headers;
    headers += L"Connection: Keep-Alive\r\n";
    headers += L"Content-Type: application/x-protobuf\r\n";
    headers += L"Accept: */*\r\n";
    headers += L"Expect: \r\n";
    if (!entitlement_token.empty()) {
        std::wstring wt(entitlement_token.begin(), entitlement_token.end());
        headers += L"Authorization: Bearer " + wt + L"\r\n";
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
        "B puuid=" + puuid.substr(0, 8));

    BOOL ok = WinHttpSendRequest(hR, headers.c_str(), (DWORD)-1L,
        (LPVOID)envelope.data(), (DWORD)envelope.size(), (DWORD)envelope.size(), 0);
    if (!ok || !WinHttpReceiveResponse(hR, nullptr)) {
        Log("[GW] Send/recv failed err=" + std::to_string(GetLastError()));
        WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS); return;
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
    WinHttpCloseHandle(hR); WinHttpCloseHandle(hC); WinHttpCloseHandle(hS);

    if (status == 200) {
        Log("[GW] *** HTTP 200 SESSION ESTABLISHED OK *** body=" + std::to_string(body.size()) + "B");
    }
    else if (status == 401) {
        Log("[GW] HTTP 401 Unauthorized body=" + std::to_string(body.size()) + "B");
        if (!body.empty()) {
            std::string s(body.begin(), body.end());
            Log("[GW] 401 body: " + s.substr(0, 200));
        }
    }
    else if (status == 400) {
        Log("[GW] HTTP 400 Bad Request body=" + std::to_string(body.size()) + "B");
        if (!body.empty()) {
            std::string s(body.begin(), body.end());
            Log("[GW] 400 body: " + s.substr(0, 200));
        }
    }
    else if (status == 500) {
        Log("[GW] HTTP 500 body=" + std::to_string(body.size()) + "B");
        if (!body.empty()) {
            std::string s(body.begin(), body.end());
            Log("[GW] 500 body: " + s.substr(0, 200));
        }
        std::ostringstream hex;
        hex << "[GW] 500 hex: ";
        for (size_t i = 0; i < (std::min)(body.size(), (size_t)32); i++)
            hex << std::hex << std::setfill('0') << std::setw(2) << (int)body[i] << " ";
        Log(hex.str());
    }
    else {
        Log("[GW] HTTP " + std::to_string(status) + " body=" + std::to_string(body.size()) + "B");
        if (!body.empty()) {
            std::string s(body.begin(), body.end());
            Log("[GW] body: " + s.substr(0, 200));
        }
    }
}

// ── VPS tunnel ────────────────────────────────────────────────────────────────

static void SendViaVPS(const std::string& rso_jwt, const std::string& id_jwt,
    const std::string& sid, const std::string& puuid,
    uint32_t pid)
{
    Log("[ENT] Fetching entitlement token...");
    std::string entitlement_token = FetchIdJwt(rso_jwt);
    if (entitlement_token.empty()) {
        Log("[ENT] Failed — aborting"); return;
    }
    if (id_jwt.empty()) Log("[IDT] No ID token — continuing without it");
    Log("[VPS] Connecting " + std::string(VPS_HOST) + ":" + std::to_string(VPS_PORT));
    TlsSocket tls;
    if (!tls.Connect(VPS_HOST, VPS_PORT, TLS_SKIP_VERIFY)) {
        Log("[VPS] TLS connect failed"); return;
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
        Log("[VPS] RSA keygen failed"); tls.Close(); return;
    }

    std::string region = "la    ";
    auto sa_payload = BuildSessionAuth(
        entitlement_token,
        puuid, sid, region, pid, hwid, rsa_pem,
        cpu_brand, cpu_model, gpu_brand, gpu_model, cpu_cores);

    auto sa_pkt = PackMsg(MSG_SESSION_AUTH, sa_payload);
    Log("[VPS] Sending SESSION_AUTH entitlement_len=" + std::to_string(entitlement_token.size()) +
        " puuid=" + puuid + " sid=" + sid);

    try {
        tls.SendAll(sa_pkt.data(), sa_pkt.size());

        auto msg = tls.RecvMsg();
        uint32_t mt = ReadU32BE(msg.data());
        uint32_t plen = ReadU32BE(msg.data() + 4);
        Log("[VPS] Received msg type=" + std::to_string(mt) + " plen=" + std::to_string(plen));

        if (mt == MSG_ERROR) {
            std::string err(msg.begin() + 8, msg.end());
            Log("[VPS] Error: " + err); tls.Close(); return;
        }
        if (mt != MSG_SESSION_AUTH_OK) {
            Log("[VPS] Expected SESSION_AUTH_OK(15), got " + std::to_string(mt));
            tls.Close(); return;
        }

        std::vector<uint8_t> payload(msg.begin() + 8, msg.end());
        auto envelope = ParseSessionAuthOk(payload);
        tls.Close();

        if (envelope.empty()) { Log("[VPS] Empty gateway envelope"); return; }

        Log("[VPS] Got envelope " + std::to_string(envelope.size()) + "B — posting to Riot");
        PostToGateway(envelope, puuid, entitlement_token, id_jwt);
    }
    catch (const std::exception& e) {
        Log("[VPS] Exception: " + std::string(e.what())); tls.Close();
    }
}

// ── Pipe extraction ───────────────────────────────────────────────────────────

static void TryExtractAndSend(const uint8_t* buf, DWORD len) {
    if (g_api_called.exchange(true)) return;

    std::string ascii(len, ' ');
    for (DWORD i = 0; i < len; i++)
        if (buf[i] >= 0x20 && buf[i] < 0x7F) ascii[i] = (char)buf[i];

    static const std::regex jwt_re(
        R"((eyJ[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}))");
    static const std::regex uuid_re(
        R"([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})");

    // Extract ALL JWTs
    std::vector<std::string> all_jwts;
    std::sregex_iterator jit(ascii.begin(), ascii.end(), jwt_re), jend;
    for (; jit != jend; ++jit) all_jwts.push_back((*jit)[1].str());

    if (all_jwts.empty()) {
        Log("[PIPE] No JWT found"); g_api_called.store(false); return;
    }

    std::string jwt = all_jwts[0];
    std::string id_jwt = all_jwts.size() > 1 ? all_jwts[1] : "";

    Log("[PIPE] JWT count: " + std::to_string(all_jwts.size()));
    Log("[PIPE] JWT: " + jwt.substr(0, 50) + "...");
    if (!id_jwt.empty()) Log("[PIPE] ID_JWT: " + id_jwt.substr(0, 50) + "...");

    std::string first_uuid, last_uuid;
    std::sregex_iterator it(ascii.begin(), ascii.end(), uuid_re), end;
    for (; it != end; ++it) {
        if (first_uuid.empty()) first_uuid = it->str();
        last_uuid = it->str();
    }
    std::string puuid = first_uuid;
    std::string sid = last_uuid;
    uint32_t    pid = g_valorant_pid;

    Log("[PIPE] PUUID: " + puuid);
    Log("[PIPE] SID:   " + sid);
    Log("[PIPE] PID:   " + std::to_string(pid));

    std::thread([jwt, id_jwt, sid, puuid, pid]() {
        SendViaVPS(jwt, id_jwt, sid, puuid, pid);
        }).detach();
}

// ── Pipe server ───────────────────────────────────────────────────────────────

static void HandleClient(HANDLE pipe) {
    std::vector<uint8_t> buf(16384);
    DWORD bytesRead; int hb_count = 0;
    while (!g_shutdown.load()) {
        if (!ReadFile(pipe, buf.data(), (DWORD)buf.size(), &bytesRead, nullptr)
            || bytesRead == 0) break;
        char sz[64]; sprintf_s(sz, "[PIPE] %u bytes (0x%02X)", bytesRead, buf[0]); Log(sz);
        if (bytesRead == 40 && buf[0] == 0x03) {
            std::vector<uint8_t> resp(buf.data(), buf.data() + 40); resp[0] = 0x04;
            DWORD bw = 0; WriteFile(pipe, resp.data(), 40, &bw, nullptr);
            sprintf_s(sz, "[PIPE] HB ack #%d", ++hb_count); Log(sz); continue;
        }
        if (bytesRead > 100) { Log("[PIPE] Scanning..."); TryExtractAndSend(buf.data(), bytesRead); }
        if (bytesRead >= 4) {
            uint32_t magic; memcpy(&magic, buf.data(), 4); uint32_t nm = magic + 1;
            std::vector<uint8_t> echo(buf.data(), buf.data() + bytesRead);
            memcpy(echo.data(), &nm, 4);
            DWORD bw = 0; WriteFile(pipe, echo.data(), (DWORD)echo.size(), &bw, nullptr);
        }
    }
    CloseHandle(pipe); Log("[PIPE] Client disconnected");
}

static void PipeServer() {
    while (!g_shutdown.load()) {
        HANDLE pipe = CreateNamedPipeW(PIPE_NAME, PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 1048576, 1048576, 500, nullptr);
        if (pipe == INVALID_HANDLE_VALUE) { Sleep(1000); continue; }
        Log("[PIPE] Waiting for client...");
        if (ConnectNamedPipe(pipe, nullptr) || GetLastError() == ERROR_PIPE_CONNECTED) {
            Log("[PIPE] Client connected");
            std::thread(HandleClient, pipe).detach();
        }
        else { CloseHandle(pipe); }
    }
}

// ── Utilities ─────────────────────────────────────────────────────────────────

static uint32_t GetValorantPID() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W pe; pe.dwSize = sizeof(pe); uint32_t pid = 0;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"VALORANT-Win64-Shipping.exe") == 0) {
                pid = pe.th32ProcessID; break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap); return pid;
}

BOOL WINAPI CtrlHandler(DWORD t) {
    if (t == CTRL_C_EVENT || t == CTRL_CLOSE_EVENT) { g_shutdown.store(true); return TRUE; }
    return FALSE;
}

// ── Main ──────────────────────────────────────────────────────────────────────

int main() {
    SetConsoleTitleW(L"vClient V5");
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

    Log("[VGC] Stopping...");
    system("sc stop vgc >nul 2>&1");
    Sleep(1200);
    Log("[VGC] Starting...");
    system("sc start vgc >nul 2>&1");
    Sleep(1200);

    HANDLE h = CreateFileW(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0,
        nullptr, OPEN_EXISTING, 0, nullptr);
    if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); Log("[VGC] Pipe intercepted"); }
    else Log("[VGC] Pipe not yet available");

    std::thread(PipeServer).detach();

    Log("Waiting for Valorant...");
    while (!g_shutdown.load()) {
        g_valorant_pid = GetValorantPID();
        if (g_valorant_pid) {
            char msg[64]; sprintf_s(msg, "[+] Valorant PID=%u", g_valorant_pid); Log(msg); break;
        }
        Sleep(500);
    }

    Log("Ready — waiting for pipe messages (Ctrl+C to exit)");
    while (!g_shutdown.load()) Sleep(500);
    Log("Shutdown");
    return 0;
}