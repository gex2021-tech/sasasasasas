# TUTORIAL COMPLETO - VGC Emulator (Setup, Configuración y Diagnóstico)

Guía paso a paso para la instalación, configuración, ejecución y resolución de problemas del emulador.

---

## 📋 PARTE 1: INSTALACIÓN INICIAL

### Paso 1.1: Instalar dependencias de Python

Abre **PowerShell** o la consola de comandos en el directorio raíz del proyecto:

```powershell
cd C:\ruta\a\amulator
pip install -r requirements.txt
```

**Componentes que instala:**
- `pyyaml`: Para lectura y actualización de `config.yaml`.
- `cryptography` / `pycryptodome`: Criptografía HMAC-SHA256, HKDF y AES-GCM.
- `requests`: Para autenticación HTTP/HTTPS con Riot Vanguard Gateway.

---

### Paso 1.2: Compilar `vClient.exe` (Si se modifica `server/main.cpp`)

```powershell
.\build_vclient.ps1
```

**Proceso:**
- Compila `server/main.cpp` generando el ejecutable `vClient.exe`.
- Implementa el **Dual-Cache Fix** (`g_cached_hb_payload` vs `g_cached_driver_status`).
- Si ya cuentas con `vClient.exe` precompilado y sin cambios en C++, puedes omitir este paso.

---

### Paso 1.3: Certificados TLS para la conexión TÚNEL

```powershell
openssl req -x509 -newkey rsa:2048 -keyout certs/server.key -out certs/server.pem -days 365 -nodes
```

**Archivos generados:**
- [`certs/server.pem`](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/certs/server.pem): Certificado TLS público.
- [`certs/server.key`](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/certs/server.key): Clave privada del servidor.

---

## ⚙️ PARTE 2: CONFIGURACIÓN (`config.yaml`)

Edita el archivo [`config.yaml`](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/config.yaml):

```yaml
tunnel:
  host: "0.0.0.0"
  port: 51820
  auth_key: "feqxYc-ilusao"
  tls_cert: "certs/server.pem"
  tls_key: "certs/server.key"
  max_clients: 32
  socket_buffer_size: 1048576
  keepalive_interval_sec: 5

client:
  server_ip: "192.168.1.136"  # IP del Servidor VPS Backend
  server_port: 51820

session:
  idle_timeout_sec: 900        # 15 Minutos seguros

heartbeat:
  interval_ms: 10000           # 10 Segundos exactos
  jitter_max_ms: 400           # ±0.4 Segundos de variación anti-replay

gateway:
  region: "la"                 # Regiones: na, la, br, eu, kr, ap
  vanguard_ua: "vanguard/1.18.3-74+20260623.212037"

stealth:
  hwid_rotation: true
  token_jitter_ms: 1500
  fallback_noise: true
```

---

## 🚀 PARTE 3: EJECUCIÓN DEL EMULADOR

### Método Recomendado (Automático):

Simplemente ejecuta en la consola de Windows:

```cmd
.\start_emulator.bat
```

O inicia el lanzador interactivo:

```powershell
python emulator_loader.py
```

### Flujo de 8 Etapas Automatizadas:
1. **Stage 0**: Inicializa el backend servidor Python (`server.main`).
2. **Stage 1**: Verifica la conexión TLS PING/PONG.
3. **Stage 2**: Detiene procesos anteriores `vgc`/`vgk` y configura el Registro de Windows.
4. **Stage 3**: Arranca `vClient.exe` y verifica que los Named Pipes (`vgk`, `vgc`, `vgservice`) estén listos.
5. **Stage 4**: Detecta la ejecución de `VALORANT-Win64-Shipping.exe`.
6. **Stage 5**: Valida la interceptación de IOCTLs.
7. **Stage 6**: Confirma el flujo de latidos y verifica que el tamaño del paquete de latido en caché sea válido (~289B).
8. **Stage 7**: Registra la sesión y abre la ventana de cola con cuenta regresiva.

---

## 🔍 PARTE 4: DIAGNÓSTICO Y AUTORREMEDIACIÓN DE ERRORES (VAL 5)

Si experimentas la desconexión `VAL 5`, ejecuta la suite de diagnóstico y autorremediación automatizada en 1 solo paso:

```cmd
.\fix_val5.bat
```

### ¿Qué ejecuta internamente?
1. **[`val5_debugger.py`](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/server/diagnostics/val5_debugger.py)**: Inspecciona los logs `logs/server.log` y `vClient.log` analizando:
   - Tiempos e intervalos de latidos.
   - Contaminación de caché dual.
   - Secuencia de llamadas IOCTL.
   - Autenticación con Vanguard Gateway.
   - Estructura Protobuf (`OSInfo variant=1`).
2. **[`val5_auto_remediate.py`](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/server/fixes/val5_auto_remediate.py)**: Lee el informe generado en `logs/val5_diagnostic.json` y aplica automáticamente las correcciones en la configuración y parámetros del servidor.
