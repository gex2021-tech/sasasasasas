# Contexto Completo del Proyecto: VGC Emulator (amulator)

**Fecha de actualizacion**: 2026-08-06
**Estado global**: En desarrollo activo. VAL 5 fix aplicado, pendiente rebuild de vClient.exe.

---

## 1. Arquitectura General y Proposito

Sistema de emulacion 2-PC para bypassear Riot Vanguard (anti-cheat de VALORANT). Emula las comunicaciones IPC entre VALORANT/Riot Client y el driver vgk.sys mediante Named Pipes locales + un servidor backend remoto que genera respuestas criptograficas realistas.

### PC Gaming (Cliente Local - Windows)

| Componente | Archivo | Funcion |
|---|---|---|
| **Loader GUI** | `emulator_loader.py` | Tkinter UI. UNICO punto de entrada. Arranca server, vClient, Riot Client, verifica heartbeats, muestra queue window. |
| **vClient** | `server/main.cpp` -> `vClient.exe` | C++ binary. Crea Named Pipes. Intercepta IPC del juego. Relay TLS al VPS. |
| **Config local** | Registry + Services | Escribe HKLM Riot Vanguard, configura servicios vgc (stopped) y vgk (kernel driver). |

### Servidor VPS (Backend - 192.168.1.136:51820)

| Componente | Archivo | Funcion |
|---|---|---|
| **Entry point** | `server/main.py` | Inicia TunnelServer TLS, SessionManager, HeartbeatRelay, Van84Monitor, WineManager. |
| **Tunnel** | `server/tunnel_server.py` | TLS TCP server port 51820. Maneja SESSION_AUTH, IOCTL, PING/PONG, JWT_UPDATE, PIPE_AUTH, SYNC. |
| **Sessions** | `server/session_manager.py` | Crea sesiones, asigna perfiles HW (pool 500 machines), purga duplicados por PUUID, gateway auth via SmartGatewayMinty. |
| **Heartbeats** | `server/heartbeat_scheduler.py` | Scheduler por sesion (interval 10s, jitter 500ms). HeartbeatRelay despacha IOCTLs. |
| **VGC Driver** | `server/vgc_driver.py` | Emula vgk.sys: 7 IOCTLs (0x222000 heartbeat, 0x22C0EC driver status, 0x222004 integrity, 0x222008 attestation, etc.) |
| **Gateway** | `server/gateway_envelope.py` | SmartGatewayMinty: mints entitlement/ID/access tokens localmente. Builds protobuf gateway envelope con F1/F15 tokens, OSInfo. |
| **Tokens** | `server/vgc_tokens.py` | F1 token (6 componentes: nonce + 2x HMAC-SHA512 + timestamp + hw_blob). F15 = Base64(SHA1(F1 + version + suffix)). |
| **Crypto** | `server/vgc_crypto.py` | CryptoSession: HMAC key derivation, per-heartbeat noise (XOR rolling HMAC), FALLBACK_TOKEN (293 bytes). |
| **HB Tasks** | `server/heartbeat_tasks.py` | Decrypt 293-byte blobs via HKDF+AES-GCM. Parse protobuf tasks. Build TaskResultRequest (type 11). |
| **Protocol** | `server/protocol.py` | Wire protocol: header !II (msg_type, payload_len). 16 message types. SessionAuthData with 22 fields. |
| **Wine/IPC** | `server/wine_manager.py` | Container management. Embedded crypto fallback (no Wine needed). Delegates IOCTL to vgc_driver. |
| **Machine Pool** | `server/machine_pool.py` | Pool de 500 perfiles de hardware para anti-fingerprinting. |

---

## 2. Configuracion (config.yaml)

```yaml
tunnel:
  host: "0.0.0.0"
  port: 51820
  auth_key: "feqxYc-ilusao"
  tls_cert: "certs/server.pem"
  tls_key: "certs/server.key"
  max_clients: 32

client:
  server_ip: "192.168.1.136"
  server_port: 51820

session:
  idle_timeout_sec: 300          # 5 min (match VAL 5 timeout)

heartbeat:
  riot_timeout_ms: 3000          # 3s response timeout
  waning_threshold_sec: 240      # 4 min (before VAL 5)
  interval_ms: 10000             # 10s heartbeat interval
  jitter_max_ms: 500             # 0.5s jitter

gateway:
  region: "la"
  vanguard_ua: "vanguard/1.18.3-74+20260623.212037"

stealth:
  hwid_rotation: true
  token_jitter_ms: 1500
  fallback_noise: true
```

---

## 3. Protocolo de Comunicacion (Wire Protocol)

Header: struct.pack("!II", msg_type, payload_length)

| Type | Name | Direction | Purpose |
|------|------|-----------|---------|
| 3 | SYNC | Client->Server | Reconnect, replay missed heartbeats |
| 4 | IOCTL | Client->Server | Forward IOCTL request (code + data) |
| 5 | IOCTL_RESP | Server->Client | IOCTL response data |
| 6 | HEARTBEAT_BUFFER | Server->Client | Proactive heartbeat push |
| 7 | PING | Client->Server | Keepalive check |
| 8 | PONG | Server->Client | Keepalive response |
| 9 | ERROR | Server->Client | Error message |
| 10 | JWT_UPDATE | Client->Server | Refresh JWT token |
| 12 | PIPE_AUTH | Client->Server | Pipe authentication |
| 14 | SESSION_AUTH | Client->Server | Full session establishment |
| 15 | SESSION_AUTH_OK | Server->Client | Session created + gateway envelope |
| 16 | TASK_RESULT | Client->Server | Heartbeat task responses |

---

## 4. Flujo de Ejecucion Completo (Loader v2.0)

El loader (emulator_loader.py) es el UNICO punto de entrada. Secuencia de 8 stages:

```
Stage 0: Start backend server (python -m server.main)
  - Si ya responde PING, lo reutiliza
  - Si no, lanza subprocess, espera hasta 15s
  
Stage 1: Verify server connection (PING/PONG TLS)

Stage 2: Kill stale processes + configure services
  - sc stop vgc, taskkill vgc.exe/vgk.sys
  - Registry: HKLM\SOFTWARE\Riot Games, Inc\Riot Vanguard
  - Services: vgc (demand/stopped), vgk (kernel/started)

Stage 3: Launch vClient.exe + RiotClientServices.exe
  - vClient creates Named Pipes and TLS tunnel to VPS
  - Riot Client launches VALORANT

Stage 4: Wait for VALORANT-Win64-Shipping.exe (max 5 min)

Stage 5: VGC bypass verification
  - Check vClient alive + parse vClient.log for session/IOCTL markers
  - Fallback: protocol PING to server

Stage 6: Establish heartbeats
  - Verify log shows IOCTL activity OR server responds to PING

Stage 7: Auth verification
  - Confirm session exists via log + PING
  - Save auth timestamp to data/loader_state.json

-> Ready Screen: Queue window countdown (240s), refresh cooldown (60s)
```

---

## 5. Named Pipes (vClient.exe)

vClient crea y escucha en estos pipes simultaneamente:

- \\.\pipe\933823D3-C77B-4BAE-89D7-A92B567236BC
- \\.\pipe\933823D3-C77B-4BAE-89D2-A92B567236BC
- \\.\pipe\vgservice
- \\.\pipe\vgc
- \\.\pipe\vgk
- \\.\pipe\OffsetPipe
- \\.\pipe\MyInjectorPipe123
- \\.\pipe\A2F16F88-3D9E-43A3-B1DA-8C9DE8B7F8E4

### Pipe Message Handling

| Byte 0x | Response | Purpose |
|---------|----------|---------|
| 0x01 | 0x02 STATUS_OK | Status query |
| 0x03 | Cached heartbeat (g_cached_hb_payload or FALLBACK_TOKEN) | VGC heartbeat ping |
| 0x04 | Cached heartbeat (same) | VGK heartbeat ping |
| 0x05 | 0x06 AUTH_OK | Auth/token check |
| 0x07 | 0x08 | Generic query |
| 0x64 | Session Init response | Session initialization |
| 0x65 | Token Exchange response | Token exchange + extract JWTs |
| 0x67 | Echo with 0x66 | In-game struct type 1 |

---

## 6. Error VAL 5 -- Root Cause y Fix Aplicado

### Root Cause (ENCONTRADO 2026-08-06)

Bug critico en server/main.cpp: Una sola variable cache (g_cached_hb_payload) servia dos propositos incompatibles:

1. El keepalive loop mandaba IOCTL 0x22C0EC (driver status, ~110 bytes protobuf) cada 10s
2. La respuesta se guardaba en g_cached_hb_payload
3. Cuando Valorant pedia heartbeat por pipe (byte 0x03/0x04), recibia el driver status protobuf
4. Valorant esperaba un gateway heartbeat token (~289-293 bytes) -- formato completamente diferente
5. Format mismatch -> timeout -> VAL 5

### Fix Aplicado

Separacion de caches en server/main.cpp:

```cpp
// ANTES (bug):
static std::vector<uint8_t> g_cached_hb_payload;   // contaminada con 0x22C0EC

// DESPUES (fix):
static std::vector<uint8_t> g_cached_hb_payload;   // SOLO gateway heartbeat (0x222000)
static std::vector<uint8_t> g_cached_driver_status; // SOLO driver status (0x22C0EC)
static std::atomic<uint32_t> g_hb_cache_ver(0);     // increments on heartbeat update
static std::atomic<uint32_t> g_driver_status_ver(0); // increments on driver status update
```

Keepalive loop dual:
- Tick par (cada 10s): Manda IOCTL 0x222000 -> respuesta a g_cached_hb_payload
- Tick impar (cada 10s): Manda IOCTL 0x22C0EC -> respuesta a g_cached_driver_status
- Ambos intercalados cada 5s base

Pipe handlers 0x03/0x04 ahora reciben correctamente el gateway heartbeat token.

### Fixes anteriores de VAL 5 (ya implementados)

1. OSInfo variant=1 (no 6, que triggerea VAL 5)
2. F1/F15 tokens (6-component structure con HMAC-SHA512)
3. IOCTL 0x22C0EC handler (driver status report)
4. Windows Security Flags (HVCI, IOMMU, SecureBoot, VBS, TPM2 = 0x1F)
5. Heartbeat task decryption (HKDF + AES-GCM)
6. Config optimizado (interval 10s, waning 240s, idle 300s)
7. First heartbeat inmediato (force=True post SESSION_AUTH)
8. Purge duplicate sessions por PUUID

---

## 7. IOCTL Codes Soportados (vgc_driver.py)

| Code | Constant | Handler | Response Size |
|------|----------|---------|---------------|
| 0x222000 | IOCTL_VGK_HEARTBEAT | _heartbeat() | ~80-100 bytes |
| 0x22C03C | IOCTL_VGK_ACCESS | _access_check() | ~8 bytes |
| 0x222004 | IOCTL_VGK_INTEGRITY | _integrity_check() | ~50 bytes |
| 0x222008 | IOCTL_VGK_ATTESTATION | _attestation() | ~80 bytes |
| 0x22200C | IOCTL_VGK_MEMORY_SCAN | _memory_scan() | ~20 bytes |
| 0x222010 | IOCTL_VGK_MODULE_CHECK | _module_check() | ~15 bytes |
| 0x22C0EC | IOCTL_VGK_DRIVER_STATUS | _driver_status() | ~110 bytes |

Todos retornan protobuf con HMAC-SHA256 signature.

---

## 8. Gateway Envelope Structure

Generado en build_gateway_envelope():

```
Field 1: version (varint) = 1
Field 2: signed_token (bytes) = "RG" + session_data + HMAC
Field 3: client_info (embedded) = region, puuid, build, RSA key
Field 4: timestamp (fixed64)
Field 5: os_info (embedded) = platform=1, arch=2, version="10.0.19045", variant=1
Field 15: F15 token (string) = Base64(SHA1(F1 + version + suffix))
```

CRITICO: variant=1 (Windows Pro). variant=6 triggerea VAL 5.

---

## 9. Crypto Flow

```
SESSION_AUTH:
  1. Client sends JWT + HWID + PUUID + build info
  2. Server derives AES key: HMAC-SHA256(jwt, hwid_hex)
  3. Server builds gateway envelope with F1/F15 tokens
  4. Server sends SESSION_AUTH_OK with envelope

HEARTBEAT (every 10s):
  1. Scheduler tick -> send_heartbeat(ioctl=0x222000)
  2. CryptoSession.ioctl_response() -> vgc_driver.handle_driver_ioctl()
  3. Response: protobuf with version, status, timestamp, scan_count, HMAC sig, security flags, OSInfo
  4. Per-heartbeat noise: XOR bytes 20-280 with rolling HMAC key

KEEPALIVE (vClient side):
  1. Every 5s alternates: 0x222000 (heartbeat) and 0x22C0EC (driver status)
  2. Heartbeat response -> g_cached_hb_payload -> pipe 0x03/0x04 responses
  3. Driver status -> g_cached_driver_status (separate cache)
```

---

## 10. Estructura de Archivos Completa

```
amulator/
+-- emulator_loader.py           # UNICO entry point (Tkinter GUI, v2.0)
+-- config.yaml                  # Configuracion centralizada
+-- vClient.exe                  # Binary C++ (pre-built)
+-- vClient.log                  # Runtime log del binary
+-- build/
|   +-- vClient.exe
+-- build_vclient.ps1            # PowerShell build script
+-- data/
|   +-- loader_state.json        # Auth state persistence
+-- certs/
|   +-- server.pem               # TLS cert
|   +-- server.key               # TLS key
+-- logs/
|   +-- server.log               # Backend server log
|   +-- server_stdout.log        # Server stdout (from loader)
|   +-- sessions/*.jsonl         # Per-session event logs
+-- server/
|   +-- __init__.py
|   +-- main.py                  # Backend entry (python -m server.main)
|   +-- main.cpp                 # vClient source code (C++)
|   +-- tunnel_server.py         # TLS TCP server
|   +-- session_manager.py       # Session lifecycle
|   +-- heartbeat_scheduler.py   # HB timing + relay
|   +-- vgc_driver.py            # IOCTL emulation (7 codes)
|   +-- vgc_crypto.py            # Crypto session + noise
|   +-- vgc_tokens.py            # F1/F15 token generation
|   +-- gateway_envelope.py      # Gateway auth + keepalive
|   +-- heartbeat_tasks.py       # HKDF+AES-GCM task decrypt
|   +-- protocol.py              # Wire protocol definition
|   +-- riot_proxy.py            # RiotProxy (HB dispatch)
|   +-- wine_manager.py          # Container + embedded crypto
|   +-- machine_pool.py          # 500-machine HW profile pool
|   +-- program_worker.py        # program.exe IPC worker
|   +-- program.cpp              # program.exe source
|   +-- event_log.py             # Session event logging
|   +-- fallback_cache.py        # HB fallback cache
|   +-- config.py                # YAML config loader
|   +-- jwt_util.py              # JWT parsing helpers
|   +-- van84_monitor.py         # VAN 84 timeout monitor
|   +-- version.py               # Version string
|   +-- banner.py                # Startup banner
+-- docs/
|   +-- VAL5_PREVENTION.md       # VAL 5 timeout analysis
|   +-- VAL5_FIXES_APPLIED.md    # Technical fix reference
|   +-- CHANGES.md               # Full changelog
|   +-- README_EMULATOR.md       # Main documentation
|   +-- LOADER_README.md         # Loader documentation
+-- validate_emulator.py         # Automated validation (6 tests)
```

---

## 11. Errores Resueltos

| Error | Causa | Solucion | Estado |
|-------|-------|----------|--------|
| VAN 83 | Registry/driver no detectados | Loader escribe registry + sc start vgk | RESUELTO |
| VAN -72 | Delay 22s + dummy auth | Sync reducido a 3s, bloqueo de probe sessions | RESUELTO |
| VAL 5 | Cache contaminada (driver status en heartbeat pipe) | Dual cache: g_cached_hb_payload + g_cached_driver_status | FIX APLICADO, PENDIENTE REBUILD |
| VAL 5 (prev) | OSInfo variant=6 | Cambiado a variant=1 | RESUELTO |
| VAL 5 (prev) | Missing F1/F15 tokens | Implementados 6-component tokens | RESUELTO |
| VAL 5 (prev) | Missing IOCTL 0x22C0EC | Handler completo implementado | RESUELTO |
| VAL 5 (prev) | Heartbeat interval 30s | Reducido a 10s | RESUELTO |

---

## 12. Pendientes / Next Steps

1. REBUILD vClient.exe con los cambios de dual-cache en server/main.cpp
2. Test end-to-end: Arrancar loader -> verificar que heartbeat cache tenga ~289 bytes (no ~110)
3. Monitorear logs post-fix: buscar [KEEPALIVE] HEARTBEAT OK y verificar que pipe reports cache=server con tamano correcto
4. Polynomial hash constants (baja prioridad): Research indica que vgc.exe valida hashes con 16 POS_MULS + 14 NEG_MULS que cambian por patch
5. Delayed ban monitoring: Observar si hay ban pattern de 10-15 min post-queue

---

## 13. Build Instructions

### vClient.exe (requiere MSVC/MinGW en Windows):
```powershell
.\build_vclient.ps1 -VpsHost "192.168.1.136" -AuthKey "feqxYc-ilusao"
```

### Backend Server:
```bash
pip install pyyaml cryptography
python -m server.main
```

### Full Stack (via Loader - metodo recomendado):
```bash
python emulator_loader.py
# El loader arranca todo automaticamente
```

---

## 14. Notas Importantes para la IA

- El loader es el UNICO entry point. No usar start_emulator.ps1 ni python -m server.main directamente.
- server/main.cpp es el source de vClient.exe -- cualquier cambio requiere recompilacion.
- La separacion de caches (heartbeat vs driver_status) es el fix critico de VAL 5.
- Los IOCTL responses son protobuf custom (no googleapis) -- se encodean manualmente.
- variant=1 en OSInfo es load-bearing. Cambiar a 6 = VAL 5 instantaneo.
- El FALLBACK_TOKEN_RAW (293 bytes en main.cpp) es una captura real de un heartbeat de VGC -- se usa como fallback cuando el server no ha respondido aun.
- El config.yaml vive en la raiz del proyecto y es leido tanto por el server Python como por el loader.
- hmac.new() en Python es correcto (no es un bug) -- el modulo hmac usa hmac.new(key, msg, digestmod).
- Los tokens F1 tienen exactamente 166 bytes (16+64+64+0+6+16). F15 es siempre 28 chars Base64.
- El server Python usa embedded crypto (CryptoSession) como fallback cuando program.exe/Wine no esta disponible.
