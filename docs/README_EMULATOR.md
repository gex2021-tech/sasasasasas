# VGC Emulator - Fixed & Enhanced + VAL 5 Fixes

## Latest Improvements (VAL 5 Research)

### **NEW: VAL 5 Error Fixes** (2026-08-03)

Based on community research, implemented critical fixes to prevent VAL 5 error during queue:

1. **F1/F15 Token Generation** (`server/vgc_tokens.py`) ⭐ NEW
   - Proper 6-component F1 token structure (nonce + 2x HMAC-SHA512 + timestamp + hw_blob)
   - F15 token as `Base64(SHA1(F1 + version + suffix))`
   - Session-bound with PUUID/HWID

2. **OSInfo Field in Gateway Envelope**
   - Field 5 added with `platform=1, arch=2, version=10.0.19045`
   - **Critical**: `variant=1` (NOT 6, which triggers VAL 5!)
   - Proper Windows version reporting

3. **IOCTL 0x22C0EC - Driver Status** ⭐ NEW
   - vgc.exe calls this when queueing
   - Reports driver_loaded, version, boot_time, protection_enabled
   - Includes Windows security features (HVCI, IOMMU, SecureBoot, VBS, TPM2)

4. **Windows Security Flags in Heartbeat**
   - Field 6 added to IOCTL 0x222000 responses
   - Reports all security features enabled (0b11111)

5. **Heartbeat Task Processing** (`server/heartbeat_tasks.py`) ⭐ NEW
   - HKDF+AES-GCM decryption for 293-byte server task blobs
   - TaskResultRequest (message type 16) responses
   - Proper task parsing and result generation

6. **Optimized Config for VAL 5 Prevention**
   - Heartbeat interval: 30s → 15s (more frequent)
   - Waning threshold: 7min → 4min (before VAL 5 timeout)
   - Idle timeout: 10min → 5min (match VAL 5 window)

### **Previous Improvements**

### 1. **VGC Driver Emulation** (`server/vgc_driver.py`)
- Realistic IOCTL response generation
- Supports all VGC driver operations:
  - `0x222000` - Heartbeat with system state + security flags
  - `0x22C03C` - Access permission checks
  - `0x222004` - Integrity verification
  - `0x22200C` - Memory region scanning
  - `0x222010` - Module verification
  - `0x22C0EC` - **Driver status report** ⭐ NEW (VAL 5 fix)
- Protobuf-encoded responses with HMAC signatures
- Per-session state tracking
- Anti-detection noise generation

### 2. **Enhanced Gateway Envelope** (`server/gateway_envelope.py`)
- Dynamic protobuf generation instead of static fallback
- Session-specific signed tokens with HMAC
- Includes client info (region, PUUID, build version)
- RSA public key embedding
- Proper protobuf field encoding

### 3. **Improved Crypto Integration** (`server/vgc_crypto.py`)
- Integrated with new driver system
- Session-bound AES key derivation
- Per-heartbeat token noise via HMAC
- Better JWT/HWID binding

### 4. **vClient Build System** (`build_vclient.ps1`)
- PowerShell build script with configurable parameters
- Auto-patches main.cpp for dynamic configuration
- No more hardcoded IPs
- Generates run script for easy testing

## Quick Start

### Server (Python Emulator)

1. **Install dependencies:**
   ```bash
   pip install -r requirements.txt
   ```
   **Note**: Now includes `cryptography` for HKDF+AES-GCM (heartbeat tasks)

2. **Configure** `config.yaml`:
   ```yaml
   tunnel:
     host: "0.0.0.0"
     port: 51820
     auth_key: "feqxYc-ilusao"  # Change this!
     tls_cert: "certs/server.pem"
     tls_key: "certs/server.key"
   
   heartbeat:
     interval_ms: 15000           # 15s (optimized for VAL 5)
     waning_threshold_sec: 240    # 4min (before VAL 5 timeout)
   
   session:
     idle_timeout_sec: 300        # 5min (match VAL 5 window)
   ```

3. **Generate TLS certificates** (if not exists):
   ```powershell
   # Self-signed cert for testing
   openssl req -x509 -newkey rsa:2048 -keyout certs/server.key -out certs/server.pem -days 365 -nodes -subj "/CN=vgc-emulator"
   ```

4. **Validate VAL 5 fixes:**
   ```bash
   python validate_emulator.py
   # Should pass all tests including new VAL 5 fixes
   ```

5. **Start server:**
   ```bash
   python -m server.main
   ```

### Client (vClient)

1. **Build with your server IP:**
   ```powershell
   .\build_vclient.ps1 -VpsHost "YOUR_SERVER_IP" -AuthKey "feqxYc-ilusao"
   ```

2. **Run vClient:**
   ```powershell
   .\run_vclient.bat
   ```
   
   Or manually:
   ```powershell
   .\build\vClient.exe
   ```

### Testing

Test the server with the Python test client:
```bash
python test_client.py --host YOUR_SERVER_IP --port 51820
```

Expected output:
```
Connecting to YOUR_SERVER_IP:51820...
[+] Connected.
[*] Sent SESSION_AUTH. Waiting for response...
[+] SESSION_AUTH_OK received. Session ID: 8b3f4c2a...
[*] Sent PING...
[+] PONG received.
[*] Sent IOCTL...
[+] IOCTL_RESP received.
```

## Architecture

```
┌─────────────────────┐
│  Gaming PC          │
│                     │
│  VALORANT.exe       │
│       ↓             │
│  vClient.exe        │◄─── Intercepts VGC pipe
│       │             │     Generates HWID
│       │ TLS         │     Fetches JWT
│       ↓             │
└─────────┬───────────┘
          │
          │ Port 51820 (TLS)
          │ SESSION_AUTH Protocol
          ↓
┌─────────────────────┐
│  Server PC          │
│                     │
│  Python Emulator    │
│       │             │
│       ├─ TunnelServer (TLS)
│       ├─ SessionManager
│       ├─ VGCDriver   │◄─── NEW: Realistic IOCTL responses
│       ├─ RiotProxy   │
│       └─ HeartbeatScheduler
│                     │
└─────────────────────┘
```

## Protocol Flow

1. **Session Establishment**
   ```
   Client → SESSION_AUTH (JWT, HWID, RSA key, build info)
   Server → SESSION_AUTH_OK (session_id, gateway_envelope)
   ```

2. **Heartbeat Loop**
   ```
   Server → HEARTBEAT_BUFFER (VGC state, integrity checks)
   Client → IOCTL (0x222000, driver commands)
   Server → IOCTL_RESP (protobuf with attestation)
   ```

3. **JWT Updates**
   ```
   Client → JWT_UPDATE (new token, PUUID)
   Server → JWT_OK
   ```

## Configuration Options

### `config.yaml`

```yaml
tunnel:
  host: "0.0.0.0"          # Listen address
  port: 51820               # TLS port
  auth_key: "YOUR-KEY"      # Change this!
  max_clients: 32          # Max simultaneous clients

session:
  idle_timeout_sec: 600    # 10 minutes

heartbeat:
  riot_timeout_ms: 5000    # Riot API timeout
  van84_threshold_sec: 420 # 7 minutes (Van84 error)
  interval_ms: 30000       # 30 seconds
  jitter_max_ms: 2500      # Random delay

gateway:
  region: "la"             # Latin America
  vanguard_ua: "vanguard/1.18.3-74+20260623.212037"
  build_branch: "release-13.00-shipping-30-4955671"
  build_changelist: 4955671

stealth:
  hwid_rotation: true      # Rotate HWID per session
  token_jitter_ms: 1500    # Token generation jitter
  fallback_noise: true     # Enable noise in responses
```

### Build Parameters

```powershell
.\build_vclient.ps1 `
    -VpsHost "YOUR_IP" `
    -VpsPort 51820 `
    -AuthKey "YOUR_AUTH_KEY" `
    -BuildType Release
```

## What's Fixed

### ❌ Before
1. **Gateway envelope**: Static fallback token
2. **IOCTL responses**: Simple echo or empty
3. **vClient**: Hardcoded IP `192.168.1.136`
4. **Crypto**: Basic stubs
5. **Driver**: No realistic behavior

### ✅ After
1. **Gateway envelope**: Dynamic protobuf with session binding
2. **IOCTL responses**: Full VGC driver simulation with integrity checks
3. **vClient**: Configurable via build script
4. **Crypto**: HMAC-based session keys, per-heartbeat noise
5. **Driver**: Complete IOCTL handler with attestation

## Testing Checklist

- [ ] Server starts without errors
- [ ] vClient connects successfully
- [ ] SESSION_AUTH completes
- [ ] Heartbeats are received
- [ ] IOCTL responses are non-empty
- [ ] JWT updates work
- [ ] Session persists for 10+ minutes
- [ ] Multiple clients can connect

## Troubleshooting

### "Connection refused"
- Check firewall rules
- Verify server is listening: `netstat -an | findstr 51820`
- Test with test_client.py first

### "auth_failed"
- Check auth_key matches in config.yaml and vClient
- Rebuild vClient with correct key

### "jwt_empty"
- vClient requires valid JWT from Riot
- For testing, use test_client.py with fake JWT

### "program.exe missing"
- Wine integration is optional
- Server works without it using embedded crypto

### vClient crashes immediately
- Check vClient.log for errors
- Verify named pipe is available
- Ensure VGC service is stopped

## Next Steps

1. **Test with real Valorant**
   - Stop VGC service
   - Run vClient
   - Launch Valorant
   - Check for VAL 5 error

2. **Monitor logs**
   ```
   logs/sessions/  - Session events
   vClient.log     - Client errors
   ```

3. **Tune heartbeat interval**
   - Adjust in config.yaml
   - Monitor Van84 threshold

4. **Add custom IOCTL handlers**
   - Edit `server/vgc_driver.py`
   - Add new IOCTL codes

## License

This is for educational/research purposes only.
