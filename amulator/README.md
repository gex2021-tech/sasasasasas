# VGC Emulator "Esperanza"

Emulador de servidores de Valorant (VGC) para testing de cheats y desarrollo.

## Estructura

```
amulator/
├── src/
│   ├── main.cpp              # Punto de entrada
│   ├── core/
│   │   └── session_manager.h  # Gestión de sesiones
│   ├── network/
│   │   └── vgc_socket.h       # Socket server
│   └── crypto/
│       └── crypto_manager.h   # Criptografía RSA
├── certs/
│   ├── emulator_private.key   # Clave privada RSA
│   ├── emulator_public.key    # Clave pública RSA
│   └── emulator_cert.pem      # Certificado TLS
├── config/
│   └── emulator_config.json   # Configuración
└── build/
    └── build.bat              # Script de compilación
```

## Requisitos

- Windows 10/11
- Visual Studio 2022 con carga de trabajo C++
- Python 3.9+ (opcional, para scripts)

## Instalación

1. **Generar certificados** (si no los tienes):
   ```powershell
   openssl genrsa -out emulator_private.key 2048
   openssl req -new -x509 -key emulator_private.key -out emulator_cert.pem -days 3650
   openssl x509 -pubkey -noout -in emulator_cert.pem > emulator_public.key
   ```

2. **Compilar**:
   ```cmd
   cd build
   build.bat
   ```

3. **Ejecutar**:
   ```cmd
   vgc_emulator.exe
   ```

## Configuración

Editar `config/emulator_config.json`:
- `port`: Puerto del servidor (default: 51820)
- `auth_key`: Clave de autenticación
- `session.timeout`: Timeout de sesiones inactivas

## Uso con Cheats

1. Iniciar el emulador en PC1 (servidor)
2. Configurar DNS/hosts en PC2 para redirigir tráfico VGC
3. Inyectar cheat que use la clave pública del emulador
4. Los tokens firmados por el emulador serán válidos

## Próximos Pasos

- Implementar parsing de paquetes VGC reales
- Añadir offsets específicos de Valorant
- Mocking de respuestas de servidores Riot
- Packet sniffer para debugging

## Notas

- Las claves RSA incluidas son de ejemplo, generar nuevas para producción
- Requiere dos PCs para testing completo (servidor + cliente)
- Error VAN 102 se fixea rotando HWID y limpiando sesiones
