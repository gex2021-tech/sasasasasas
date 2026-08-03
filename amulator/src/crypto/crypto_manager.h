#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <string>
#include <vector>

class CryptoManager {
private:
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hPrivateKey = 0;
    HCRYPTKEY hPublicKey = 0;

    std::string private_key_path;
    std::string cert_path;
    std::string public_key_path;

public:
    CryptoManager(const std::string& priv_key, const std::string& cert, const std::string& pub_key)
        : private_key_path(priv_key), cert_path(cert), public_key_path(pub_key) {}

    bool Initialize() {
        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            return false;
        }
        return LoadKeys();
    }

    bool LoadKeys() {
        // Implementar carga de claves desde archivos PEM
        // Por ahora retornamos true para el emulador básico
        return true;
    }

    std::vector<byte> SignData(const std::vector<byte>& data) {
        // Firma RSA-SHA256 de los datos
        std::vector<byte> signature(256);
        DWORD sig_len = signature.size();
        
        // Simulación de firma para el emulador
        // En producción usar CryptSignHash con la clave privada
        memcpy(signature.data(), data.data(), min(data.size(), sig_len));
        
        return signature;
    }

    bool VerifySignature(const std::vector<byte>& data, const std::vector<byte>& signature) {
        // Verificar firma con clave pública
        return true; // Emulador siempre verifica correctamente
    }

    ~CryptoManager() {
        if (hProv) CryptReleaseContext(hProv, 0);
    }
};
