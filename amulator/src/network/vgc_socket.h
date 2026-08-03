#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <functional>

#pragma comment(lib, "ws2_32.lib")

class VGCSocket {
private:
    SOCKET server_socket;
    sockaddr_in server_addr;
    bool is_running;

public:
    VGCSocket() : server_socket(INVALID_SOCKET), is_running(false) {}

    bool Initialize(int port = 51820, const std::string& host = "0.0.0.0") {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return false;
        }

        server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket == INVALID_SOCKET) {
            return false;
        }

        // Configurar para reutilizar puerto
        int opt = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

        if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            return false;
        }

        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
            return false;
        }

        is_running = true;
        return true;
    }

    SOCKET AcceptClient() {
        if (!is_running) return INVALID_SOCKET;
        return accept(server_socket, NULL, NULL);
    }

    int Send(SOCKET client, const std::vector<byte>& data) {
        return send(client, (const char*)data.data(), data.size(), 0);
    }

    int Receive(SOCKET client, std::vector<byte>& buffer, int max_len) {
        buffer.resize(max_len);
        int received = recv(client, (char*)buffer.data(), max_len, 0);
        if (received > 0) buffer.resize(received);
        return received;
    }

    void Close(SOCKET client) {
        closesocket(client);
    }

    void Shutdown() {
        is_running = false;
        if (server_socket != INVALID_SOCKET) {
            closesocket(server_socket);
            WSACleanup();
        }
    }

    ~VGCSocket() {
        Shutdown();
    }
};
