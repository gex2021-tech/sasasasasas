// VGC Emulator "Esperanza" - Servidor Principal
// Compilar con: cl main.cpp /EHsc /std:c++17 /O2 /link ws2_32.lib

#include <iostream>
#include <thread>
#include <atomic>
#include "network/vgc_socket.h"
#include "core/session_manager.h"
#include "crypto/crypto_manager.h"

std::atomic<bool> g_running(true);
SessionManager g_sessions;
VGCSocket g_server;

void HandleClient(SOCKET client) {
    std::cout << "[+] Cliente conectado\n";
    
    SessionManager* sessions = &g_sessions;
    char buffer[4096];
    
    while (g_running) {
        int bytes = recv(client, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        
        // Aquí iría el parsing de paquetes VGC reales
        // Por ahora hacemos eco para testing
        send(client, buffer, bytes, 0);
    }
    
    closesocket(client);
    std::cout << "[-] Cliente desconectado\n";
}

int main() {
    std::cout << "===========================================\n";
    std::cout << "   VGC Emulator \"Esperanza\" v1.0\n";
    std::cout << "   Iniciando servidor en puerto 51820...\n";
    std::cout << "===========================================\n\n";

    if (!g_server.Initialize(51820)) {
        std::cerr << "[ERROR] No se pudo iniciar el servidor\n";
        return 1;
    }

    std::cout << "[OK] Servidor escuchando...\n";
    std::cout << "[INFO] Presiona Ctrl+C para detener\n\n";

    // Thread para limpiar sesiones inactivas
    std::thread cleanup_thread([]() {
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            g_sessions.CleanupIdleSessions(600);
        }
    });

    // Loop principal de aceptación
    while (g_running) {
        SOCKET client = g_server.AcceptClient();
        if (client != INVALID_SOCKET) {
            std::thread(HandleClient, client).detach();
        }
    }

    cleanup_thread.join();
    g_server.Shutdown();
    return 0;
}
