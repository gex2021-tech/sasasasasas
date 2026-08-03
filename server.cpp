// server.cpp - Núcleo del Emulador VGC "Esperanza"
// Compilar con: cl server.cpp /EHsc /std:c++17 /O2 /link ws2_32.lib bcrypt.lib crypt32.lib

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <bcrypt.h>
#include <wincrypt.h>

#ifndef CRYPT_DECODE_ALLOC_OBJ_INFO
#define CRYPT_DECODE_ALLOC_OBJ_INFO 0x8000
#endif
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

#define PORT 51820
#define BUFFER_SIZE 4096

// Estructura simple para manejar clientes
struct ClientInfo {
    SOCKET socket;
    std::string session_id;
    bool authenticated;
};

std::vector<ClientInfo> clients;

// Función para cargar la clave privada RSA desde un archivo PEM
BCRYPT_KEY_HANDLE LoadPrivateKey(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[-] Error: No se pudo abrir " << filename << std::endl;
        return nullptr;
    }
    
    std::string pemContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    DWORD derLen = 0;
    if (!CryptStringToBinaryA(pemContent.c_str(), 0, CRYPT_STRING_BASE64HEADER, nullptr, &derLen, nullptr, nullptr)) {
        std::cerr << "[-] Error: Formato PEM no válido en " << filename << std::endl;
        return nullptr;
    }

    std::vector<BYTE> derBuffer(derLen);
    if (!CryptStringToBinaryA(pemContent.c_str(), 0, CRYPT_STRING_BASE64HEADER, derBuffer.data(), &derLen, nullptr, nullptr)) {
        std::cerr << "[-] Error decodificando PEM" << std::endl;
        return nullptr;
    }

    DWORD capiBlobLen = 0;
    BYTE* pCapiBlob = nullptr;
    if (!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, derBuffer.data(), derLen, CRYPT_DECODE_ALLOC_OBJ_INFO, nullptr, &pCapiBlob, &capiBlobLen)) {
        PCRYPT_PRIVATE_KEY_INFO pKeyInfo = nullptr;
        DWORD keyInfoSize = 0;
        if (CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_PRIVATE_KEY_INFO, derBuffer.data(), derLen, CRYPT_DECODE_ALLOC_OBJ_INFO, nullptr, &pKeyInfo, &keyInfoSize)) {
            CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, pKeyInfo->PrivateKey.pbData, pKeyInfo->PrivateKey.cbData, CRYPT_DECODE_ALLOC_OBJ_INFO, nullptr, &pCapiBlob, &capiBlobLen);
            LocalFree(pKeyInfo);
        }
    }

    if (!pCapiBlob) {
        std::cerr << "[-] Error decodificando ASN.1 RSA: 0x" << std::hex << GetLastError() << std::endl;
        return nullptr;
    }

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RSA_ALGORITHM, nullptr, 0);
    if (status != 0) {
        std::cerr << "[-] Error abriendo BCrypt Provider: 0x" << std::hex << status << std::endl;
        LocalFree(pCapiBlob);
        return nullptr;
    }

    BCRYPT_KEY_HANDLE hKey = nullptr;
    status = BCryptImportKeyPair(hAlg, nullptr, LEGACY_RSAPRIVATE_BLOB, &hKey, pCapiBlob, capiBlobLen, 0);
    LocalFree(pCapiBlob);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (status != 0) {
        std::cerr << "[-] Error importando clave privada: 0x" << std::hex << status << std::endl;
        return nullptr;
    }

    std::cout << "[+] Clave privada cargada correctamente." << std::endl;
    return hKey;
}

// Función principal de manejo de cliente
void HandleClient(SOCKET clientSocket, BCRYPT_KEY_HANDLE hPrivKey) {
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    std::cout << "[*] Nuevo cliente conectado desde socket: " << clientSocket << std::endl;

    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        // Aquí es donde ocurre la magia:
        // 1. Parsear el paquete entrante (opcode, payload)
        // 2. Generar respuesta falsa pero válida
        // 3. Firmar la respuesta con tu clave RSA privada
        
        std::cout << "[*] Recibidos " << bytesReceived << " bytes." << std::endl;
        
        // SIMULACIÓN DE RESPUESTA (Handshake básico)
        // En una implementación real, aquí decodificarías el protocolo de Valorant
        std::string response = "HTTP/1.1 200 OK\r\nX-Riot-Timestamp: " + std::to_string(GetTickCount64()) + "\r\n\r\n";
        
        // Enviar respuesta firmada (simplificado para este ejemplo)
        send(clientSocket, response.c_str(), (int)response.length(), 0);
    }

    closesocket(clientSocket);
    std::cout << "[*] Cliente desconectado." << std::endl;
}

int main() {
    WSADATA wsaData;
    SOCKET listenSocket, clientSocket;
    struct addrinfo *result = NULL, hints;

    std::cout << "🚀 Iniciando Emulador VGC 'Esperanza'..." << std::endl;

    // 1. Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[-] Error WSAStartup" << std::endl;
        return 1;
    }

    // 2. Cargar Claves RSA
    BCRYPT_KEY_HANDLE hPrivKey = LoadPrivateKey("emulator_private.key");
    if (!hPrivKey) {
        std::cerr << "[-] Crítico: Sin claves RSA, el emulador no puede firmar tokens." << std::endl;
        return 1;
    }

    // 3. Configurar Socket
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, std::to_string(PORT).c_str(), &hints, &result);
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    
    bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    listen(listenSocket, SOMAXCONN);
    
    std::cout << "[+] Escuchando en puerto " << PORT << "..." << std::endl;
    std::cout << "[+] Esperando conexiones de clientes (cheats/launcher)..." << std::endl;

    // 4. Bucle Principal
    while (true) {
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) continue;
        
        // Manejar cada cliente en un hilo separado (simplificado aquí como secuencial para demo)
        HandleClient(clientSocket, hPrivKey);
    }

    WSACleanup();
    return 0;
}
