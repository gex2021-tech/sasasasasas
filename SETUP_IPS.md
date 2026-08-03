# Configuración de IPs - Setup de 2 PCs

## 🖥️ Tu Setup Actual

### PC 1: Gaming PC (Donde corre Valorant + vClient)
- **Función**: Ejecuta Valorant y vClient
- **IP**: Variable (cliente)
- **Software**: Windows + Valorant + vClient.exe

### PC 2: Server PC (Donde corre el emulador Python)
- **Función**: Servidor VGC Emulator
- **IP**: `192.168.1.136` ⭐
- **Software**: Windows/Linux + Python + Emulador

## 📡 Conexión

```
┌─────────────────────────┐         ┌─────────────────────────┐
│   GAMING PC             │         │   SERVER PC             │
│                         │         │   IP: 192.168.1.136    │
│   Valorant.exe          │         │                         │
│       ↓                 │         │   Python Emulator       │
│   vClient.exe           │◄────────┤   Port: 51820 (TLS)    │
│   (configurado con      │  LAN    │   Listening on          │
│    192.168.1.136:51820) │         │   0.0.0.0:51820        │
└─────────────────────────┘         └─────────────────────────┘
```

## ⚙️ Configuración Correcta

### En el GAMING PC

#### 1. Construir vClient con IP del servidor
```powershell
.\build_vclient.ps1 -VpsHost "192.168.1.136" -AuthKey "feqxYc-ilusao"
```

Esto crea `server/vclient_config.h`:
```c
#define VPS_HOST_AUTO "192.168.1.136"
#define VPS_PORT_AUTO 51820
#define AUTH_KEY_AUTO "feqxYc-ilusao"
```

#### 2. Ejecutar vClient
```powershell
.\build\vClient.exe
# O usar: .\run_vclient.bat
```

#### 3. Loader GUI (opcional)
```powershell
python emulator_loader.py
# Auto-detecta 192.168.1.136 desde vclient_config.h
```

### En el SERVER PC (192.168.1.136)

#### 1. Configurar `config.yaml`
```yaml
tunnel:
  host: "0.0.0.0"          # Escucha en todas las interfaces
  port: 51820               # Puerto TLS
  auth_key: "feqxYc-ilusao" # DEBE COINCIDIR con vClient
```

**Nota**: `0.0.0.0` significa "escuchar en todas las IPs", permitiendo conexiones desde cualquier PC en la red local.

#### 2. Iniciar servidor
```bash
python -m server.main
```

Expected output:
```
tunnel TLS listening 0.0.0.0:51820
[*] Server ready - waiting for vClient connections
```

#### 3. Verificar firewall
```powershell
# Windows Firewall - permitir puerto 51820
New-NetFirewallRule -DisplayName "VGC Emulator" -Direction Inbound -LocalPort 51820 -Protocol TCP -Action Allow
```

## 🔍 Verificación

### Desde GAMING PC

#### Test 1: Ping al servidor
```powershell
ping 192.168.1.136
# Debe responder
```

#### Test 2: Test con Python test client
```bash
python test_client.py --host 192.168.1.136 --port 51820
```

Expected:
```
[+] Connected to 192.168.1.136:51820
[+] SESSION_AUTH successful
[+] IOCTL response received
```

#### Test 3: Verificar vClient.log
```
vClient.log:
[+] Connecting to 192.168.1.136:51820
[+] TLS connection established
[+] SESSION_AUTH_OK received
```

### Desde SERVER PC

#### Ver conexiones activas
```powershell
netstat -an | findstr 51820
```

Expected:
```
TCP    0.0.0.0:51820         0.0.0.0:0              LISTENING
TCP    192.168.1.136:51820   192.168.1.XXX:XXXXX   ESTABLISHED
```

#### Ver logs del servidor
```
logs/sessions/session_*.log:
{"type": "session_auth", "client_ip": "192.168.1.XXX", ...}
```

## ❌ Errores Comunes

### Error: "Connection refused"

**Causa**: Firewall bloqueando puerto 51820 en SERVER PC

**Solución**:
```powershell
# SERVER PC
New-NetFirewallRule -DisplayName "VGC Emulator" -Direction Inbound -LocalPort 51820 -Protocol TCP -Action Allow

# O temporalmente deshabilitar firewall
Set-NetFirewallProfile -Profile Domain,Public,Private -Enabled False
```

### Error: "Timeout connecting"

**Causa**: IP incorrecta o servidor no corriendo

**Verificar**:
1. SERVER PC está en `192.168.1.136`:
   ```powershell
   ipconfig
   # Ver "IPv4 Address"
   ```

2. Servidor Python corriendo:
   ```bash
   python -m server.main
   # Debe decir "listening 0.0.0.0:51820"
   ```

### Error: "auth_failed"

**Causa**: `auth_key` diferente entre cliente y servidor

**Solución**:
```yaml
# SERVER PC - config.yaml
tunnel:
  auth_key: "feqxYc-ilusao"  # ← Este valor

# GAMING PC - rebuild vClient
.\build_vclient.ps1 -VpsHost "192.168.1.136" -AuthKey "feqxYc-ilusao"
#                                                        ↑
#                                          DEBE SER EL MISMO
```

### Error: "Using localhost (127.0.0.1)"

**Causa**: No se encontró configuración, usando default incorrecto

**Solución**:
```powershell
# GAMING PC - rebuild con IP correcta
.\build_vclient.ps1 -VpsHost "192.168.1.136"

# Verificar que se creó vclient_config.h
cat server\vclient_config.h
# Debe decir: #define VPS_HOST_AUTO "192.168.1.136"
```

## 🎯 Quick Setup Checklist

### SERVER PC (192.168.1.136)
- [ ] Python instalado
- [ ] `config.yaml` con `host: "0.0.0.0"` y `port: 51820`
- [ ] Firewall permite puerto 51820
- [ ] Servidor corriendo: `python -m server.main`
- [ ] Ver: "tunnel TLS listening 0.0.0.0:51820"

### GAMING PC
- [ ] Compilador C++ (Visual Studio)
- [ ] Build vClient: `.\build_vclient.ps1 -VpsHost "192.168.1.136"`
- [ ] Verificar: `cat server\vclient_config.h` muestra IP correcta
- [ ] Ejecutar: `.\build\vClient.exe`
- [ ] Ver: "Connected to 192.168.1.136:51820"

### Test Final
- [ ] GAMING PC: `ping 192.168.1.136` ✓
- [ ] GAMING PC: `python test_client.py --host 192.168.1.136` ✓
- [ ] SERVER PC: `netstat -an | findstr 51820` muestra ESTABLISHED ✓
- [ ] Logs: `logs/sessions/*.log` tiene eventos ✓

## 📝 Notas Importantes

1. **La IP 192.168.1.136 es fija**: Es la IP del SERVER PC en tu red local
2. **0.0.0.0 != 192.168.1.136**: 
   - `0.0.0.0` = servidor escucha en TODAS las interfaces
   - `192.168.1.136` = IP específica del servidor en la red
3. **127.0.0.1 = localhost**: Solo para testing en mismo PC (no es tu caso)
4. **Auth key debe coincidir**: Mismo valor en `config.yaml` y build de vClient
5. **Puerto 51820**: Default, puedes cambiarlo si hay conflicto

---

**Setup actual**: 2-PC con SERVER en `192.168.1.136`  
**Última actualización**: 2026-08-03  
**Todos los archivos actualizados** para usar esta IP por default
