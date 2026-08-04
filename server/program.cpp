/*
 * program.cpp — IPC crypto server for vgk.sys emulation
 * 
 * Compilar en Windows (PC Servidor):
 *   cl program.cpp /EHsc /std:c++17 /O2 /W3 /link ws2_32.lib bcrypt.lib
 * 
 * Uso:
 *   program.exe --container <id> --ipc-port <puerto> --work-dir <ruta>
 * 
 * Protocolo IPC:
 *   - CMD_MOUNT (1): Recibe perfil JSON, inicializa contexto criptográfico
 *   - CMD_IOCTL (2): Recibe código IOCTL + datos, devuelve respuesta criptográfica
 *   - CMD_SET_JWT (3): Actualiza JWT y PUUID del perfil
 *   - CMD_PING (4): Health check
 *   - CMD_SHUTDOWN (5): Cierra limpiamente
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <bcrypt.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")

// =============================================================================
// CONFIG
// =============================================================================
constexpr uint16_t DEFAULT_PORT = 5928;
constexpr int IPC_TIMEOUT_MS = 5000;

// Comandos IPC
constexpr uint8_t CMD_MOUNT = 1;
constexpr uint8_t CMD_IOCTL = 2;
constexpr uint8_t CMD_SET_JWT = 3;
constexpr uint8_t CMD_PING = 4;
constexpr uint8_t CMD_SHUTDOWN = 5;

// Header de petición: version(1), cmd(1), flags(1), ioctl_code(4), data_len(4)
#pragma pack(push, 1)
struct RequestHeader {
    uint8_t version;
    uint8_t cmd;
    uint8_t flags;
    uint32_t ioctl_code;
    uint32_t data_len;
};
#pragma pack(pop)

// Header de respuesta: status(4), data_len(4)
#pragma pack(push, 1)
struct ResponseHeader {
    uint32_t status;
    uint32_t data_len;
};
#pragma pack(pop)

// =============================================================================
// LOGGING
// =============================================================================
static std::mutex g_log_mtx;
static void Log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(g_log_mtx);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm_buf{};
    localtime_s(&tm_buf, &now);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    std::cout << "[" << time_buf << "] " << msg << std::endl;
}

// =============================================================================
// CRYPTO CONTEXT
// =============================================================================
class CryptoContext {
public:
    CryptoContext() : mounted_(false), hb_count_(0) {}
    
    bool Mount(const std::string& profile_json) {
        std::lock_guard<std::mutex> lock(mtx_);
        profile_ = profile_json;
        mounted_ = true;
        hb_count_ = 0;
        
        // Derivar clave AES del perfil (SHA256 del JWT o del perfil completo)
        if (!profile_json.empty()) {
            DeriveKey(profile_json);
        }
        
        Log("[CRYPTO] Mounted profile, size=" + std::to_string(profile_json.size()));
        return true;
    }
    
    void UpdateJWT(const std::string& jwt, const std::string& puuid) {
        std::lock_guard<std::mutex> lock(mtx_);
        jwt_ = jwt;
        puuid_ = puuid;
        if (!jwt.empty()) {
            DeriveKey(jwt + "|" + puuid);
        }
        Log("[CRYPTO] JWT updated, puuid=" + (puuid.size() > 8 ? puuid.substr(0, 8) : puuid));
    }
    
    std::vector<uint8_t> HandleIOCTL(uint32_t ioctl_code, const std::vector<uint8_t>& input) {
        std::lock_guard<std::mutex> lock(mtx_);
        
        if (!mounted_) {
            Log("[CRYPTO] Not mounted, returning empty");
            return {};
        }
        
        // Emular respuestas de IOCTLs del driver vgk.sys
        switch (ioctl_code) {
            case 0x22C03C: // IOCTL_ACCESS
                return GenerateAccessResponse(input);
            
            case 0x222000: // IOCTL_HEARTBEAT_STUB
                return GenerateHeartbeatResponse();
            
            default:
                Log("[CRYPTO] Unknown IOCTL 0x" + ToHex(ioctl_code));
                return GenerateDefaultResponse(ioctl_code, input);
        }
    }
    
private:
    std::mutex mtx_;
    std::string profile_;
    std::string jwt_;
    std::string puuid_;
    std::vector<uint8_t> aes_key_;
    bool mounted_;
    uint32_t hb_count_;
    
    void DeriveKey(const std::string& seed) {
        BCRYPT_ALG_HANDLE hAlg = nullptr;
        BCRYPT_HASH_HANDLE hHash = nullptr;
        DWORD hashLen = 32;
        
        aes_key_.resize(32);
        
        if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) == 0) {
            if (BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0) == 0) {
                BCryptHashData(hHash, (PUCHAR)seed.data(), (ULONG)seed.size(), 0);
                BCryptFinishHash(hHash, aes_key_.data(), hashLen, 0);
                BCryptDestroyHash(hHash);
            }
            BCryptCloseAlgorithmProvider(hAlg, 0);
        }
    }
    
    std::vector<uint8_t> GenerateAccessResponse(const std::vector<uint8_t>& input) {
        // Generar respuesta dinámica basada en input + clave de sesión
        // Esto emula la operación criptográfica real del driver
        
        std::vector<uint8_t> output(293, 0);
        
        // Header Protobuf (bytes 0-19 fijos)
        static const uint8_t proto_header[] = {
            0x08, 0x01, 0x12, 0xA0, 0x02, 0x52, 0x47, 0x01, 0x00, 0x05,
            0xFA, 0xA7, 0x74, 0xC9, 0x93, 0x69, 0x50, 0x77, 0xF4, 0xB0
        };
        memcpy(output.data(), proto_header, sizeof(proto_header));
        
        // Generar payload dinámico usando HMAC-SHA256
        if (!aes_key_.empty() && !input.empty()) {
            BCRYPT_ALG_HANDLE hAlg = nullptr;
            BCRYPT_HASH_HANDLE hHash = nullptr;
            DWORD hashLen = 32;
            
            std::vector<uint8_t> hash_data = aes_key_;
            hash_data.insert(hash_data.end(), input.begin(), input.end());
            
            std::vector<uint8_t> hmac_result(32);
            
            if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) == 0) {
                if (BCryptCreateHash(hAlg, &hHash, nullptr, 0, aes_key_.data(), (ULONG)aes_key_.size(), 0) == 0) {
                    BCryptHashData(hHash, input.data(), (ULONG)input.size(), 0);
                    BCryptFinishHash(hHash, hmac_result.data(), hashLen, 0);
                    BCryptDestroyHash(hHash);
                }
                BCryptCloseAlgorithmProvider(hAlg, 0);
            }
            
            // Mezclar HMAC en el payload (bytes 20-280)
            for (int i = 20; i < 280 && i - 20 < (int)hmac_result.size(); i++) {
                output[i] = hmac_result[(i - 20) % hmac_result.size()] ^ (uint8_t)(hb_count_ & 0xFF);
            }
        } else {
            // Fallback: usar datos pseudo-aleatorios basados en timestamp
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            for (int i = 20; i < 280; i++) {
                output[i] = (uint8_t)((now >> ((i - 20) % 8)) ^ (i * 7));
            }
        }
        
        hb_count_++;
        return output;
    }
    
    std::vector<uint8_t> GenerateHeartbeatResponse() {
        // Generar token de heartbeat dinámico
        std::vector<uint8_t> token(293, 0);
        
        // Header Protobuf
        static const uint8_t proto_header[] = {
            0x08, 0x01, 0x12, 0xA0, 0x02, 0x52, 0x47, 0x01, 0x00, 0x05,
            0xFA, 0xA7, 0x74, 0xC9, 0x93, 0x69, 0x50, 0x77, 0xF4, 0xB0
        };
        memcpy(token.data(), proto_header, sizeof(proto_header));
        
        // Payload dinámico basado en contador y timestamp
        auto now = std::chrono::steady_clock::now().time_since_epoch().count();
        uint32_t counter = hb_count_++;
        
        for (int i = 20; i < 280; i++) {
            uint8_t dyn = (uint8_t)((now >> ((i % 8))) ^ (counter * (i + 1)) ^ (i * 13));
            if (!aes_key_.empty()) {
                dyn ^= aes_key_[i % aes_key_.size()];
            }
            token[i] = dyn;
        }
        
        return token;
    }
    
    std::vector<uint8_t> GenerateDefaultResponse(uint32_t ioctl_code, const std::vector<uint8_t>& input) {
        // Respuesta genérica para IOCTLs desconocidos
        std::vector<uint8_t> response(64, 0);
        
        // Copiar código IOCTL en la respuesta
        memcpy(response.data(), &ioctl_code, 4);
        memcpy(response.data() + 4, input.data(), std::min(input.size(), (size_t)56));
        
        return response;
    }
    
    std::string ToHex(uint32_t val) {
        std::ostringstream oss;
        oss << std::hex << std::uppercase << val;
        return oss.str();
    }
};

// =============================================================================
// SERVER
// =============================================================================
class IPCServer {
public:
    IPCServer(uint16_t port, const std::string& container_id)
        : port_(port), container_id_(container_id), running_(true) {}
    
    bool Start() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            Log("[SERVER] WSAStartup failed");
            return false;
        }
        
        SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSock == INVALID_SOCKET) {
            Log("[SERVER] socket failed: " + std::to_string(WSAGetLastError()));
            WSACleanup();
            return false;
        }
        
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port_);
        
        if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            Log("[SERVER] bind failed: " + std::to_string(WSAGetLastError()));
            closesocket(listenSock);
            WSACleanup();
            return false;
        }
        
        if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
            Log("[SERVER] listen failed: " + std::to_string(WSAGetLastError()));
            closesocket(listenSock);
            WSACleanup();
            return false;
        }
        
        Log("[SERVER] Listening on 127.0.0.1:" + std::to_string(port_) + 
            " container=" + container_id_.substr(0, 8));
        
        // Señalar que estamos listos
        SignalReady();
        
        // Loop principal
        while (running_) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(listenSock, &readfds);
            
            timeval tv{1, 0}; // 1 segundo timeout
            
            int ret = select(0, &readfds, nullptr, nullptr, &tv);
            if (ret > 0 && FD_ISSET(listenSock, &readfds)) {
                sockaddr_in clientAddr{};
                int clientLen = sizeof(clientAddr);
                SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientLen);
                
                if (clientSock != INVALID_SOCKET) {
                    HandleClient(clientSock);
                    closesocket(clientSock);
                }
            }
        }
        
        closesocket(listenSock);
        WSACleanup();
        return true;
    }
    
    void Stop() {
        running_ = false;
    }
    
private:
    uint16_t port_;
    std::string container_id_;
    std::atomic<bool> running_;
    CryptoContext crypto_;
    
    void SignalReady() {
        // Crear archivo de señalización
        std::string ready_file = "ready_" + container_id_ + ".txt";
        HANDLE hFile = CreateFileA(ready_file.c_str(), GENERIC_WRITE, 0, nullptr, 
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE) {
            const char* msg = "READY";
            DWORD written;
            WriteFile(hFile, msg, 5, &written, nullptr);
            CloseHandle(hFile);
        }
    }
    
    void HandleClient(SOCKET clientSock) {
        // Configurar timeout
        DWORD timeout = IPC_TIMEOUT_MS;
        setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        setsockopt(clientSock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
        
        Log("[CLIENT] Connected");
        
        while (running_) {
            // Leer header
            RequestHeader hdr{};
            int received = recv(clientSock, (char*)&hdr, sizeof(hdr), 0);
            if (received <= 0) {
                break;
            }
            
            if (hdr.version != 1) {
                Log("[CLIENT] Invalid version: " + std::to_string(hdr.version));
                break;
            }
            
            // Leer datos
            std::vector<uint8_t> data(hdr.data_len);
            if (hdr.data_len > 0) {
                int total = 0;
                while (total < (int)hdr.data_len) {
                    int r = recv(clientSock, (char*)data.data() + total, hdr.data_len - total, 0);
                    if (r <= 0) break;
                    total += r;
                }
                if (total != (int)hdr.data_len) {
                    Log("[CLIENT] Incomplete data read");
                    break;
                }
            }
            
            // Procesar comando
            std::vector<uint8_t> response = ProcessCommand(hdr.cmd, hdr.ioctl_code, data);
            
            // Enviar respuesta
            ResponseHeader resp_hdr{0, (uint32_t)response.size()};
            send(clientSock, (const char*)&resp_hdr, sizeof(resp_hdr), 0);
            if (!response.empty()) {
                send(clientSock, (const char*)response.data(), response.size(), 0);
            }
        }
        
        Log("[CLIENT] Disconnected");
    }
    
    std::vector<uint8_t> ProcessCommand(uint8_t cmd, uint32_t ioctl_code, const std::vector<uint8_t>& data) {
        switch (cmd) {
            case CMD_MOUNT: {
                std::string json(data.begin(), data.end());
                bool ok = crypto_.Mount(json);
                return ok ? std::vector<uint8_t>{1} : std::vector<uint8_t>{0};
            }
            
            case CMD_IOCTL: {
                return crypto_.HandleIOCTL(ioctl_code, data);
            }
            
            case CMD_SET_JWT: {
                // Parsear JSON simple {"jwt": "...", "puuid": "..."}
                std::string json(data.begin(), data.end());
                size_t jwt_pos = json.find("\"jwt\":");
                size_t puuid_pos = json.find("\"puuid\":");
                
                std::string jwt, puuid;
                if (jwt_pos != std::string::npos) {
                    size_t start = json.find('"', jwt_pos + 6) + 1;
                    size_t end = json.find('"', start);
                    jwt = json.substr(start, end - start);
                }
                if (puuid_pos != std::string::npos) {
                    size_t start = json.find('"', puuid_pos + 7) + 1;
                    size_t end = json.find('"', start);
                    puuid = json.substr(start, end - start);
                }
                
                crypto_.UpdateJWT(jwt, puuid);
                return std::vector<uint8_t>{1};
            }
            
            case CMD_PING: {
                return std::vector<uint8_t>{'O', 'K'};
            }
            
            case CMD_SHUTDOWN: {
                running_ = false;
                return std::vector<uint8_t>{'B', 'Y', 'E'};
            }
            
            default: {
                Log("[CMD] Unknown command: " + std::to_string(cmd));
                return std::vector<uint8_t>{0};
            }
        }
    }
};

// =============================================================================
// MAIN
// =============================================================================
int main(int argc, char* argv[]) {
    std::string container_id;
    uint16_t ipc_port = DEFAULT_PORT;
    std::string work_dir;
    
    // Parsear argumentos
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--container" && i + 1 < argc) {
            container_id = argv[++i];
        } else if (arg == "--ipc-port" && i + 1 < argc) {
            ipc_port = (uint16_t)std::atoi(argv[++i]);
        } else if (arg == "--work-dir" && i + 1 < argc) {
            work_dir = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Uso: program.exe --container <id> --ipc-port <puerto> --work-dir <ruta>\n";
            return 0;
        }
    }
    
    if (container_id.empty()) {
        std::cerr << "Error: --container es requerido\n";
        return 1;
    }
    
    Log("[INIT] Starting program.exe container=" + container_id.substr(0, 8) + 
        " port=" + std::to_string(ipc_port));
    
    IPCServer server(ipc_port, container_id);
    if (!server.Start()) {
        Log("[INIT] Server failed to start");
        return 1;
    }
    
    Log("[SHUTDOWN] Exiting gracefully");
    return 0;
}
