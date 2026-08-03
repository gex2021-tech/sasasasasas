# VGC Emulator Loader - Flow Diagram

## 📊 Nuevo Flow con Verificación de Servidor

```
┌─────────────────────────────────────────────────────────┐
│                    LOADER START                         │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│  STAGE 0: Verifying Server Connection (0% → 10%)       │
│  ┌───────────────────────────────────────────────────┐ │
│  │ 1. Read config (vclient_config.h or config.yaml) │ │
│  │ 2. Get server IP and port                        │ │
│  │ 3. Try TLS connection                            │ │
│  │ 4. Send PING message                             │ │
│  │ 5. Wait for PONG response (timeout: 5s)          │ │
│  └───────────────────────────────────────────────────┘ │
└────────────────┬────────────────────────────────────────┘
                 │
      ┌──────────┴──────────┐
      │                     │
      ▼                     ▼
   SUCCESS              FAILURE
      │                     │
      │              ┌──────▼─────────────────────────────┐
      │              │  ERROR SCREEN                      │
      │              │  ✗ SERVER NOT REACHABLE            │
      │              │                                    │
      │              │  Details:                          │
      │              │    Host: XXX.XXX.XXX.XXX          │
      │              │    Port: 51820                     │
      │              │                                    │
      │              │  [RETRY]  [EDIT CONFIG]  [Exit]   │
      │              └────────────────────────────────────┘
      │                     │
      │                     └─→ [Manual Fix] ──→ RETRY
      │
      ▼
┌─────────────────────────────────────────────────────────┐
│  STAGE 1: Killing Stale Processes (10% → 20%)          │
│  ┌───────────────────────────────────────────────────┐ │
│  │ Kill old vgc, vgk, vClient processes              │ │
│  └───────────────────────────────────────────────────┘ │
└────────────────┬────────────────────────────────────────┘
                 ▼
┌─────────────────────────────────────────────────────────┐
│  STAGE 2: Launching Riot Client (20% → 30%)            │
│  ┌───────────────────────────────────────────────────┐ │
│  │ Start vClient.exe in background                   │ │
│  └───────────────────────────────────────────────────┘ │
└────────────────┬────────────────────────────────────────┘
                 ▼
┌─────────────────────────────────────────────────────────┐
│  STAGE 3: Bypassing VGC Check (30% → 50%)              │
│  ┌───────────────────────────────────────────────────┐ │
│  │ ⏳ WAIT FOR USER TO START VALORANT                │ │
│  │                                                   │ │
│  │ Detecting VALORANT.exe in processes...           │ │
│  │ (Timeout: 5 minutes)                             │ │
│  └───────────────────────────────────────────────────┘ │
└────────────────┬────────────────────────────────────────┘
                 │
      ┌──────────┴──────────┐
      │                     │
      ▼                     ▼
  DETECTED              TIMEOUT
      │                     │
      │              ┌──────▼─────────────────────────────┐
      │              │  ERROR: Timeout waiting for game   │
      │              │  [RETRY]  [Exit]                   │
      │              └────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────┐
│  STAGE 4: Establishing Heartbeats (50% → 70%)          │
│  ┌───────────────────────────────────────────────────┐ │
│  │ Verify vClient → Server connection                │ │
│  │ Check SESSION_AUTH_OK in logs                     │ │
│  └───────────────────────────────────────────────────┘ │
└────────────────┬────────────────────────────────────────┘
                 ▼
┌─────────────────────────────────────────────────────────┐
│  STAGE 5: Sending Auth Request (70% → 100%)            │
│  ┌───────────────────────────────────────────────────┐ │
│  │ Wait for game loading screen                      │ │
│  │ Send auth request to server                       │ │
│  └───────────────────────────────────────────────────┘ │
└────────────────┬────────────────────────────────────────┘
                 ▼
┌─────────────────────────────────────────────────────────┐
│                    SUCCESS SCREEN                       │
│                                                         │
│                        ✓                                │
│              YOU CAN QUEUE NOW                          │
│                                                         │
│           [REFRESH]    [INJECT]                         │
│                                                         │
│            NEXT AUTH REQUEST                            │
│                 4:42                                    │
└─────────────────────────────────────────────────────────┘
```

## 🔍 Detalle de Verificación de Servidor

```python
def verify_server_connection(host, port, timeout=5):
    """
    Verifica que el servidor esté corriendo y respondiendo
    
    Steps:
    1. Create TLS socket
    2. Connect to server
    3. Send PING (MsgType.PING = 7)
       Format: [msg_type: uint32][payload_len: uint32]
               [     7     ][      0      ]
    
    4. Receive PONG (MsgType.PONG = 8)
       Expected: [msg_type: uint32][payload_len: uint32]
                 [     8     ][      0      ]
    
    5. Return True if PONG received, False otherwise
    """
```

### Casos de Error

| Error | Causa | Solución |
|-------|-------|----------|
| **ConnectionRefusedError** | Puerto cerrado o servidor no corriendo | Iniciar servidor: `python -m server.main` |
| **socket.timeout** | Servidor no responde en 5s | Verificar firewall, red, o IP incorrecta |
| **SSL error** | Certificados inválidos | Regenerar certs: `openssl req -x509 ...` |
| **Wrong response** | Servidor responde pero no PONG | Verificar versión del protocolo |

## 📈 Progreso Detallado

```
0%    START
      │
      ├─ Read config files
      ├─ Parse server IP/port
      ├─ TLS handshake
      └─ PING/PONG exchange
      │
10%   ✓ Server verified
      │
      ├─ Find vgc/vgk/vClient processes
      ├─ Kill stale processes
      └─ Wait 1s
      │
20%   ✓ Processes cleaned
      │
      ├─ Locate vClient.exe
      ├─ Start subprocess
      └─ Wait 2s
      │
30%   ✓ vClient running
      │
      ├─ Poll process list
      ├─ Search for "valorant"
      └─ Wait... (user action required)
      │
40%   User started Valorant
      │
      ├─ Simulate VGC bypass
      └─ Wait 2s
      │
50%   ✓ VGC bypassed
      │
      ├─ Check vClient process
      ├─ Read vClient.log
      └─ Verify SESSION_AUTH_OK
      │
70%   ✓ Heartbeats established
      │
      ├─ Wait for loading screen
      ├─ Send auth request
      └─ Wait 1s
      │
100%  ✓ READY TO PLAY
```

## ⚠️ Error Recovery

### Error en Stage 0 (Server Connection)

```
┌─────────────────────────────────────┐
│  ✗ SERVER NOT REACHABLE             │
├─────────────────────────────────────┤
│  Host: 192.168.1.136                │
│  Port: 51820                        │
│                                     │
│  Please ensure:                     │
│    • Server is running              │
│    • Firewall allows port           │
│    • IP address is correct          │
│                                     │
│  ┌────────┐  ┌──────────────┐      │
│  │ RETRY  │  │ EDIT CONFIG  │      │
│  └────────┘  └──────────────┘      │
└─────────────────────────────────────┘
```

**Actions:**
- **RETRY**: Re-run verification from Stage 0
- **EDIT CONFIG**: Opens `config.yaml` in default editor
- **Exit**: Close loader

### Error en Stage 2 (vClient Start)

```
Status: "❌ Failed to start vClient"
Action: Button START se reactiva
User: Click START to retry
```

### Error en Stage 3 (Game Timeout)

```
Status: "❌ Timeout waiting for game"
Action: Button START se reactiva
User: Start Valorant, then click START
```

## 🎨 Visual States

### State: Verifying Server (0-10%)

```
┌────────────────────────────────────┐
│          VGC EMULATOR              │
│          Esperanza v1.0            │
│                                    │
│             5%                     │
│      ┌██──────────────┐            │
│      └────────────────┘            │
│                                    │
│   ◉ Verifying server connection    │  ← Pulsing purple
│   ● Killing stale processes        │
│   ● Launching Riot client          │
│   ● Bypassing VGC check            │
│   ● Establishing heartbeats        │
│   ● Sending auth request           │
│                                    │
│  Verifying connection to           │
│  emulator server...                │
└────────────────────────────────────┘
```

### State: Server Error

```
┌────────────────────────────────────┐
│                                    │
│               ✗                    │  ← Red X (48pt)
│                                    │
│     SERVER NOT REACHABLE           │  ← Red text
│                                    │
│  Cannot connect to emulator server:│
│                                    │
│  Host: 192.168.1.136                   │
│  Port: 51820                       │
│                                    │
│  Please ensure:                    │
│    • Server is running             │
│    • Firewall allows port          │
│    • IP address is correct         │
│                                    │
│  ┌────────┐  ┌──────────────┐     │
│  │ RETRY  │  │ EDIT CONFIG  │     │
│  └────────┘  └──────────────┘     │
│                                    │
│              Exit                  │
└────────────────────────────────────┘
```

## 🧪 Testing

### Test Server Connection

```bash
# Terminal 1: Start server
python -m server.main

# Terminal 2: Test loader
python emulator_loader.py
# Click START
# Should see: "✓ Server connected: 192.168.1.136:51820"
```

### Test Server Error

```bash
# Terminal 1: DON'T start server

# Terminal 2: Test loader
python emulator_loader.py
# Click START
# Should see: "✗ SERVER NOT REACHABLE"
```

### Test Demo Mode

```bash
# No server needed
python demo_loader.py
# Click START
# Auto-simulates server connection
```

## 📝 Config Priority

Loader reads server config in this order:

1. **`server/vclient_config.h`** (build-time config)
   ```c
   #define VPS_HOST_AUTO "192.168.1.100"
   #define VPS_PORT_AUTO 51820
   ```

2. **`config.yaml`** (server config)
   ```yaml
   tunnel:
     host: "0.0.0.0"  # Converted to 192.168.1.136 for client
     port: 51820
   ```

3. **Default**: `192.168.1.136:51820`

## ✅ Beneficios de Stage 0

1. **Early Detection**: Detecta servidor apagado ANTES de iniciar vClient
2. **Clear Feedback**: Muestra error específico con IP/puerto
3. **Easy Recovery**: Botones RETRY y EDIT CONFIG
4. **No Confusion**: Usuario sabe exactamente qué está mal
5. **Time Saving**: No espera a Valorant si el servidor no funciona

---

**Actualizado**: 2026-08-03  
**Nueva Feature**: Stage 0 - Server Verification  
**Progress**: 0% → 10% → 20% → 30% → 50% → 70% → 100%
