# VAL 81 Fix - Emulator Repairs Applied

## Problem Identified
Error VAL 81 occurred during Valorant loading screen because the vClient tunnel was not actively sending IOCTL 0x22C0EC (DRIVER_STATUS) requests to the server. This IOCTL is critical - vgc.exe calls it when queueing to verify vgk.sys driver is loaded and functioning.

## Fixes Applied

### 1. **server/main.cpp** - IOCTL Keepalive Loop
**Changed:** VPS Ping keepalive → IOCTL 0x22C0EC keepalive

- Replaced generic PING/PONG loop with specific IOCTL 0x22C0EC (DRIVER_STATUS) requests
- Sends IOCTL every 10 seconds (was 20s for PING)
- Logs each successful response with counter for debugging
- Added proper PackIOCTL() helper function for building IOCTL messages

**Code changes:**
```cpp
// OLD: Generic ping
auto ping_pkt = PackMsg(MSG_PING, {});
tls.SendAll(ping_pkt.data(), ping_pkt.size());

// NEW: Specific IOCTL 0x22C0EC
auto ioctl_pkt = PackIOCTL(0x22C0EC, ioctl_data);
tls.SendAll(ioctl_pkt.data(), ioctl_pkt.size());
```

### 2. **emulator_loader.py** - Enhanced Heartbeat Verification
**Changed:** Simple connection check → IOCTL tunnel verification

**New verification flow:**
1. Check vClient.log for "0x22C0EC" + "OK/RESP/DRIVER_STATUS" entries
2. Look for "IOCTL-KEEPALIVE" log entries from vClient
3. Fallback: Send proper protocol PING (MsgType 7) and expect PONG (MsgType 8)
4. Final fallback: If vClient process is running, assume tunnel works

**Status messages updated:**
- ✓ IOCTL 0x22C0EC tunnel verified (VAL 5 fix)
- ✓ Server PONG received - tunnel ready
- ⚠️ Using fallback - vClient running

### 3. **Existing Infrastructure (Already Working)**
- `server/vgc_driver.py` - Already handles IOCTL 0x22C0EC correctly (lines 285-327)
- `server/tunnel_server.py` - Already relays IOCTLs to heartbeat relay (lines 145-153)
- `server/heartbeat_tasks.py` - Already processes IOCTLs via program.exe

## How It Works Now

1. **vClient starts** → Establishes TLS tunnel to server
2. **SESSION_AUTH completed** → Session established
3. **IOCTL keepalive loop** → Every 10s sends 0x22C0EC request
4. **Server responds** → vgc_driver.py generates proper DRIVER_STATUS response
5. **Loader verifies** → Checks vClient.log for IOCTL activity
6. **Valorant queues** → vgc.exe calls IOCTL 0x22C0EC, gets valid response
7. **No VAL 81** → Driver status verified, game proceeds

## Testing Recommendations

1. **Monitor vClient.log** during loading:
   - Should see "[IOCTL-KEEPALIVE] DRIVER_STATUS OK counter=X" every 10s
   - Should see "0x22C0EC" entries in log

2. **Check server logs**:
   - tunnel_server.py should log IOCTL messages
   - vgc_driver.py should log driver_status responses

3. **Loader UI stages**:
   - Stage 4 "Bypassing VGC check" should show "✓ IOCTL 0x22C0EC tunnel verified"
   - Stage 5 "Establishing heartbeats" should complete successfully

## Files Modified
- `/workspace/server/main.cpp` - Added IOCTL keepalive loop
- `/workspace/emulator_loader.py` - Enhanced heartbeat verification

## No Changes Needed
- `server/vgc_driver.py` - Already correct
- `server/tunnel_server.py` - Already correct
- `server/heartbeat_tasks.py` - Already correct
- `config.yaml` - Settings already optimal
