#pragma once
#include <string>
#include <map>
#include <chrono>

struct ClientSession {
    std::string client_id;
    std::string access_token;
    std::string entitlement_token;
    uint64_t session_id;
    std::chrono::steady_clock::time_point last_activity;
    bool is_authenticated;
    
    ClientSession() : session_id(0), is_authenticated(false) {
        last_activity = std::chrono::steady_clock::now();
    }
};

class SessionManager {
private:
    std::map<std::string, ClientSession> sessions;
    uint64_t next_session_id;

public:
    SessionManager() : next_session_id(1000) {}

    std::string CreateSession(const std::string& client_id) {
        ClientSession session;
        session.client_id = client_id;
        session.session_id = next_session_id++;
        session.access_token = GenerateFakeToken("access");
        session.entitlement_token = GenerateFakeToken("entitlement");
        session.is_authenticated = true;
        
        sessions[client_id] = session;
        return session.access_token;
    }

    ClientSession* GetSession(const std::string& client_id) {
        auto it = sessions.find(client_id);
        if (it != sessions.end()) {
            it->second.last_activity = std::chrono::steady_clock::now();
            return &it->second;
        }
        return nullptr;
    }

    void RemoveSession(const std::string& client_id) {
        sessions.erase(client_id);
    }

    void CleanupIdleSessions(int timeout_seconds) {
        auto now = std::chrono::steady_clock::now();
        for (auto it = sessions.begin(); it != sessions.end();) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - it->second.last_activity).count();
            if (elapsed > timeout_seconds) {
                it = sessions.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::string GenerateFakeToken(const std::string& type) {
        // Generar token falso válido para el emulador
        return "vgc_emulator_" + type + "_" + std::to_string(next_session_id++);
    }
};
