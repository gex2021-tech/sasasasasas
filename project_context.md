# Contexto Completo del Proyecto: VGC Emulator (amulator)

**Fecha de actualización**: 2026-08-06
**Estado global**: **PRODUCCIÓN / ESTABLE**. Refactorización integral de seguridad, criptografía anti-replay, peticiones HTTPS reales a Vanguard Gateway, planificador stealth y adaptación dinámica de versiones del juego completada en todos los módulos backend.

---

## 1. Arquitectura General y Propósito

Sistema de emulación 2-PC para bypass de Riot Vanguard (anti-cheat de VALORANT). Emula las comunicaciones IPC entre VALORANT/Riot Client y el driver `vgk.sys` mediante Named Pipes locales + un servidor backend remoto que procesa la autenticación y genera respuestas criptográficas realistas.

### PC Gaming (Cliente Local - Windows)

| Componente | Archivo | Función |
|---|---|---|
| **Loader GUI** | `emulator_loader.py` | Tkinter UI v3.0. ÚNICO punto de entrada. Inicia backend, vClient, Riot Client, pre-valida Named Pipes (`vgk`, `vreg`, `vgservice`), valida la caché de latidos (`_validate_heartbeat_cache`) y gestiona la ventana de cola. |
| **vClient** | `server/main.cpp` -> `vClient.exe` | Binario C++ con Dual-Cache Fix (`g_cached_hb_payload` 0x222000 vs `g_cached_driver_status` 0x22C0EC). Intercepta IPC del juego y realiza Relay TLS al VPS. |
| **Config local** | Registry + Services | Escribe HKLM Riot Vanguard, configura servicios `vgc` (demand/stopped) y `vgk` (kernel driver). |

### Servidor VPS (Backend - 192.168.1.136:51820)

| Componente | Archivo | Función |
|---|---|---|
| **Entry point** | `server/main.py` | Inicia TunnelServer TLS, SessionManager, HeartbeatRelay, Van84Monitor, WineManager. |
| **Tunnel** | `server/tunnel_server.py` | Servidor TLS TCP en puerto 51820. Maneja bounds de mensajes, límite 1MB de payload, prevención de secuestro en `SYNC`, timeouts de socket y logs avanzados de desconexión SSL. |
| **Sessions** | `server/session_manager.py` | Gestiona el ciclo de vida de sesiones, almacena `machine_profile` del pool de 500 máquinas, parsea `build_info` dinámico de VALORANT, efectúa auth con el Gateway y destruye estados de driver en `destroy_session`. |
| **Heartbeats** | `server/heartbeat_scheduler.py` | Planificador stealth (intervalo 10s, jitter de cálculo único por intervalo). Mutación de secuencias con thread-locking, búfer de reconexión `SYNC` validado y snapshot de métricas. |
| **VGC Driver** | `server/vgc_driver.py` | Emula `vgk.sys`: 7 IOCTLS (0x222000, 0x22C0EC, 0x222004, 0x222008, etc.) con Protobuf en orden estrictamente ascendente, firma HMAC calculada antes del campo 8, límite de 2KB por respuesta y `cleanup_session()`. |
| **Gateway** | `server/gateway_envelope.py` | Peticiones HTTPS reales a `https://{region}.vg.ac.pvp.net/vanguard/v1/gateway?action=3` con reintentos para 429/5xx, trazado SHA-1, `OSInfo` `variant=1` (Windows Pro) y sobre Protobuf con tokens F1/F15 reales. |
| **Tokens** | `server/vgc_tokens.py` | F1 token (166 bytes exactos: nonce + 2x HMAC-SHA512 + timestamp + hw_blob) con derivación de clave por `entitlements_token`/`HWID`. F15 = Base64(SHA1(F1 + version + suffix)). |
| **Crypto** | `server/vgc_crypto.py` | `CryptoSession`: Derivación HMAC de clave de sesión (JWT + HWID), ruido por latido anti-replay (microsegundos + salt de 4B `os.urandom(4)` XOR bytes 20..280) y validación de opcodes permitidos. |
| **HB Tasks** | `server/heartbeat_tasks.py` | Desencriptación de tareas en latidos mediante HKDF + AES-GCM (293B) y respuesta `TaskResultRequest` (tipo 16). |
| **Protocol** | `server/protocol.py` | Protocolo Wire: header `!II` (msg_type, payload_len). 16 tipos de mensajes. `SessionAuthData` extendido con soporte para `entitlements_token` e `id_token`. |
| **Wine/IPC** | `server/wine_manager.py` | Gestión de contenedores y fallback a criptografía embebida (`CryptoSession`). |
| **Machine Pool** | `server/machine_pool.py` | Pool de 500 perfiles de hardware para anti-fingerprinting. |

---

## 2. Configuración (`config.yaml`)

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

## 3. Protocolo de Comunicación (Wire Protocol)

Header: `struct.pack("!II", msg_type, payload_length)`

| Type | Name | Direction | Purpose |
|------|------|-----------|---------|
| 3 | SYNC | Client->Server | Reconnect, replay missed heartbeats (session_id validated) |
| 4 | IOCTL | Client->Server | Forward IOCTL request (code + data) |
| 5 | IOCTL_RESP | Server->Client | IOCTL response data |
| 6 | HEARTBEAT_BUFFER | Server->Client | Proactive heartbeat push |
| 7 | PING | Client->Server | Keepalive check |
| 8 | PONG | Server->Client | Keepalive response |
| 9 | ERROR | Server->Client | Error message |
| 10 | JWT_UPDATE | Client->Server | Refresh JWT token |
| 12 | PIPE_AUTH | Client->Server | Pipe authentication |
| 14 | SESSION_AUTH | Client->Server | Full session establishment (with dynamic build_info) |
| 15 | SESSION_AUTH_OK | Server->Client | Session created + gateway envelope |
| 16 | TASK_RESULT | Client->Server | Heartbeat task responses |

---

## 4. Flujo de Ejecución Completo (`emulator_loader.py` v3.0)

`emulator_loader.py` es el ÚNICO punto de entrada recomendado. Secuencia de 8 stages:

```
Stage 0: Pre-validate config + Start backend server (python -m server.main)
  - Si responde PING, lo reutiliza.
  - Si no, lanza subprocess y espera readiness.
  
Stage 1: Verify server connection (PING/PONG TLS).

Stage 2: Kill stale processes + configure services.
  - sc stop vgc, taskkill vgc.exe/vgk.sys
  - Registry: HKLM\SOFTWARE\Riot Games, Inc\Riot Vanguard
  - Services: vgc (demand/stopped), vgk (kernel/started)

Stage 3: Pre-check Named Pipes readiness (\\.\pipe\vgk, vgc, vgservice).
  - Launch vClient.exe + RiotClientServices.exe

Stage 4: Wait for VALORANT-Win64-Shipping.exe (max 5 min).

Stage 5: VGC bypass verification.
  - Parse vClient.log for session/IOCTL markers (IOCTL 0x222000 vs 0x22C0EC).

Stage 6: Establish heartbeats + validate heartbeat cache size (_validate_heartbeat_cache).
  - Verfica que la respuesta en caché sea de ~289-293 bytes (Gateway Token) y NUNCA ~110 bytes.

Stage 7: Auth verification + cache session state in data/session_state.json.

-> Ready Screen: Queue window countdown (240s), refresh cooldown (60s).
```

---

## 5. Named Pipes (`vClient.exe`)

`vClient` escucha y responde en estos pipes simultáneamente:

- `\\.\pipe\933823D3-C77B-4BAE-89D7-A92B567236BC`
- `\\.\pipe\933823D3-C77B-4BAE-89D2-A92B567236BC`
- `\\.\pipe\vgservice`
- `\\.\pipe\vgc`
- `\\.\pipe\vgk`
- `\\.\pipe\OffsetPipe`
- `\\.\pipe\MyInjectorPipe123`
- `\\.\pipe\A2F16F88-3D9E-43A3-B1DA-8C9DE8B7F8E4`

### Dual-Cache Fix (`vClient.cpp`)

| Variable de Caché | Propósito | Tamaño Esperado | Pipe de Respuesta |
|---|---|---|---|
| `g_cached_hb_payload` | Gateway Heartbeat Token (IOCTL `0x222000`) | ~289 - 293 bytes | Pipe `0x03` y `0x04` |
| `g_cached_driver_status` | Estado de Driver Protobuf (IOCTL `0x22C0EC`) | ~110 bytes | Keepalive interno / Consultas de estado |

---

## 6. Error VAL 5 — Solución Completa e Integrada

### Causas Raíz Eliminadas:

1. **Contaminación de Caché Dual**: Separación en `vClient.cpp` de `g_cached_hb_payload` vs `g_cached_driver_status`.
2. **Tokens Falsos de Gateway**: `post_gateway_auth` efectúa solicitudes HTTPS reales a los endpoints oficiales de Riot (`https://{region}.vg.ac.pvp.net`). Se eliminó el padding artificial con `=`.
3. **Inversión de Orden en Protobuf**: `vgc_driver.py` codifica los campos en orden ascendente (Campo 6 `kernel_integrity_level` antes del Campo 7 `OSInfo`).
4. **Cálculo de Firma HMAC**: Se calcula sobre el mensaje Protobuf *antes* de adjuntar el campo de firma 8.
5. **OSInfo `variant`**: Fijado a `variant=1` (Windows 10/11 Pro).
6. **Versiones del Juego**: `Session.build_info` extrae dinámicamente la versión mayor, menor y parche transmitida por `vClient` desde los binarios del juego, adaptándose automáticamente a actualizaciones de VALORANT.

---

## 7. IOCTL Codes Soportados (`vgc_driver.py`)

| Code | Constant | Handler | Purpose |
|------|----------|---------|---------|
| `0x222000` | `IOCTL_VGK_HEARTBEAT` | `_heartbeat()` | Latido principal de estado |
| `0x22C03C` | `IOCTL_VGK_ACCESS` | `_access_check()` | Verificación de permisos de acceso |
| `0x222004` | `IOCTL_VGK_INTEGRITY` | `_integrity_check()` | Verificación de integridad de archivos |
| `0x222008` | `IOCTL_VGK_ATTESTATION` | `_attestation()` | Atestiguamiento criptográfico con reto |
| `0x22200C` | `IOCTL_VGK_MEMORY_SCAN` | `_memory_scan()` | Escaneo de regiones de memoria (quick/deep) |
| `0x222010` | `IOCTL_VGK_MODULE_CHECK` | `_module_check()` | Verificación de módulos cargados |
| `0x22C0EC` | `IOCTL_VGK_DRIVER_STATUS` | `_driver_status()` | Informe de estado de `vgk.sys` para `vgc.exe` |

---

## 8. Estructura de Archivos del Proyecto

```
amulator/
+-- emulator_loader.py           # UNICO entry point (Tkinter GUI v3.0)
+-- config.yaml                  # Configuración centralizada
+-- vClient.exe                  # Binario C++ compilado
+-- vClient.log                  # Log en tiempo de ejecución de vClient
+-- build_vclient.ps1            # Script de compilación de vClient.cpp
+-- project_context.md           # Documento de contexto del proyecto
+-- requirements.txt             # Dependencias de Python
+-- start_emulator.bat           # Launcher script batch
+-- data/
|   +-- loader_state.json        # Persistencia de estado de autenticación
|   +-- session_state.json       # Estado de sesión en caché
+-- certs/
|   +-- server.pem               # Certificado TLS
|   +-- server.key               # Clave privada TLS
+-- logs/
|   +-- server.log               # Log principal del backend VPS
|   +-- sessions/*.jsonl         # Registros de eventos por sesión
+-- server/
|   +-- main.py                  # Entrada del backend (python -m server.main)
|   +-- main.cpp                 # Código fuente C++ de vClient
|   +-- tunnel_server.py         # Servidor TLS TCP con parches de seguridad
|   +-- session_manager.py       # Gestión de ciclo de vida y Gateway Auth
|   +-- heartbeat_scheduler.py   # Scheduler stealth con jitter y thread-locking
|   +-- vgc_driver.py            # Emulación IOCTL con orden Protobuf corregido
|   +-- vgc_crypto.py            # Sesión criptográfica con ruido microsegundos+salt
|   +-- vgc_tokens.py            # Generación de tokens F1 (166B) y F15
|   +-- gateway_envelope.py      # HTTPS Gateway auth + envelope Protobuf
|   +-- heartbeat_tasks.py       # Desencriptación HKDF+AES-GCM de tareas
|   +-- protocol.py              # Definición del protocolo Wire y SessionAuthData
|   +-- riot_proxy.py            # Dispatcher RiotProxy
|   +-- wine_manager.py          # Gestión de contenedores y fallback embebido
|   +-- machine_pool.py          # Pool de 500 perfiles HW de máquinas
|   +-- config.py                # Cargador de configuración YAML
|   +-- jwt_util.py              # Parser de JWT
+-- validate_emulator.py         # Pruebas de validación automatizadas (8/8 PASS)
```

---

## 9. Instrucciones de Ejecución

### Ejecución Directa Recomendada:
```cmd
start_emulator.bat
```
*(Lanza `emulator_loader.py` que administra automáticamente el backend Python, los Named Pipes, la verificación de parches y la cola de ingreso sin necesidad de recompilaciones).*
