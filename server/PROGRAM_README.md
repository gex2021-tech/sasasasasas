# DOCUMENTACIÓN: program.exe - Componente Anti-VAL 5

## ¿QUÉ ES program.exe?

`program.exe` es un **servidor IPC (Inter-Process Communication)** que emula las operaciones criptográficas del controlador `vgk.sys` de Riot Vanguard. Su propósito principal es generar respuestas dinámicas y únicas para los desafíos criptográficos que el cliente de VALORANT envía al driver, evitando así el **Error VAL 5**.

---

## ¿POR QUÉ EXISTE?

### El Problema (Error VAL 5)

Cuando VALORANT inicia, el anti-cheat Vanguard realiza verificaciones continuas mediante:

1. **IOCTLs (Input/Output Control Codes)**: Comandos que el usuario envía al driver en modo kernel
2. **Heartbeats criptográficos**: Tokens firmados que prueban que el driver está activo y funcionando
3. **Challenge-Response**: Desafíos donde el cliente envía datos y espera una respuesta específica firmada criptográficamente

En modo **STUB** (sin program.exe), el servidor usa respuestas predefinidas (`FALLBACK_TOKEN`). Esto causa VAL 5 porque:
- Las respuestas son estáticas y detectables
- No hay entropía real (variación aleatoria)
- Riot puede identificar que no hay un driver real ejecutándose

### La Solución

`program.exe` proporciona:
- ✅ **Respuestas dinámicas**: Cada IOCTL genera una respuesta única basada en:
  - Clave de sesión derivada del JWT
  - Timestamp actual
  - Contador interno de heartbeats
  - Datos de entrada del cliente
- ✅ **Entropía criptográfica**: Usa HMAC-SHA256 para mezclar datos
- ✅ **Headers Protobuf válidos**: Mantiene la estructura esperada por los servidores de Riot
- ✅ **Baja latencia**: Responde en <5ms para evitar timeouts

---

## ARQUITECTURA

```
┌─────────────────────────────────────────────────────────────────┐
│                    PC SERVIDOR (Backend/VPS)                    │
│                                                                 │
│  ┌──────────────┐     ┌──────────────┐     ┌──────────────┐   │
│  │  main.py     │────▶│ wine_manager │────▶│ program.exe  │   │
│  │  (Servidor   │     │   (Gestor    │     │  (Crypto     │   │
│  │   TLS)       │     │   de Cont.)  │     │   Server)    │   │
│  └──────────────┘     └──────────────┘     └──────────────┘   │
│                              │                      │          │
│                              │         127.0.0.1:PORT         │
│                              │         (IPC Socket)           │
│                              ▼                      │          │
│                       ┌──────────────┐             │          │
│                       │ vgc_crypto.py│◀────────────┤          │
│                       │ (Fallback)   │             │          │
│                       └──────────────┘             │          │
│                                                    │          │
└────────────────────────────────────────────────────┼──────────┘
                                                     │
                    ┌────────────────────────────────┘
                    │  Comandos IPC:
                    │  • CMD_MOUNT (1)      - Inicializar perfil
                    │  • CMD_IOCTL (2)      - Procesar IOCTL
                    │  • CMD_SET_JWT (3)    - Actualizar credenciales
                    │  • CMD_PING (4)       - Health check
                    │  • CMD_SHUTDOWN (5)   - Cerrar
                    │
                    ▼
         ┌─────────────────────┐
         │   CryptoContext     │
         │  ┌───────────────┐  │
         │  │ DeriveKey()   │  │ ← SHA256/JWT → AES Key
         │  ├───────────────┤  │
         │  │ HandleIOCTL() │  │ ← Switch según código
         │  ├───────────────┤  │
         │  │ GenerateX()   │  │ ← Respuestas dinámicas
         │  └───────────────┘  │
         └─────────────────────┘
```

---

## PROTOCOLO IPC

### Mensajes (Binario, Little Endian)

#### Petición
```
Offset  Size  Campo
------  ----  -----
0       1     version (siempre 1)
1       1     comando (1-5)
2       1     flags (reservado)
3       4     ioctl_code (solo para CMD_IOCTL)
7       4     data_len (longitud de datos)
11      N     datos (JSON o binario)
```

#### Respuesta
```
Offset  Size  Campo
------  ----  -----
0       4     status (0 = éxito)
4       4     data_len (longitud de respuesta)
8       N     datos de respuesta
```

### Comandos Soportados

| CMD | Nombre | Descripción | Entrada | Salida |
|-----|--------|-------------|---------|--------|
| 1 | `CMD_MOUNT` | Inicializa contexto criptográfico | JSON del perfil | `{1}` si OK |
| 2 | `CMD_IOCTL` | Procesa IOCTL del driver | Código + datos binarios | Respuesta criptográfica (293 bytes) |
| 3 | `CMD_SET_JWT` | Actualiza JWT y PUUID | `{"jwt":"...", "puuid":"..."}` | `{1}` |
| 4 | `CMD_PING` | Verifica que está vivo | (vacío) | `"OK"` |
| 5 | `CMD_SHUTDOWN` | Cierra el servidor | (vacío) | `"BYE"` |

---

## COMPILACIÓN

### Requisitos
- **Sistema**: Windows 10/11 (x64)
- **Compilador**: MSVC (Visual Studio 2019+)
- **Librerías**: `ws2_32.lib`, `bcrypt.lib` (incluidas en Windows SDK)

### Pasos

1. **Abrir Developer Command Prompt for VS**
   ```
   Inicio → Visual Studio → Developer Command Prompt for VS 2022
   ```

2. **Navegar a la carpeta server/**
   ```cmd
   cd C:\path\to\amulator\server
   ```

3. **Compilar**
   ```cmd
   cl program.cpp /EHsc /std:c++17 /O2 /W3 /link ws2_32.lib bcrypt.lib /OUT:program.exe
   ```

   O usar el script incluido:
   ```cmd
   compile_program.bat
   ```

### Flags de Compilación
| Flag | Propósito |
|------|-----------|
| `/EHsc` | Habilita excepciones C++ |
| `/std:c++17` | Estándar C++17 |
| `/O2` | Optimización máxima |
| `/W3` | Nivel de warnings 3 |

---

## CONFIGURACIÓN

### config.yaml (PC Servidor)

```yaml
wine:
  enabled: true              # ← Activar uso de program.exe
  program_path: "server/program.exe"
  ready_timeout_sec: 30      # Tiempo máx. para iniciar
  ipc_timeout_ms: 5000       # Timeout por operación IPC
  work_dir: "data/containers"
```

### Flujo de Inicio

1. **Python (wine_manager.py)** crea un contenedor
2. **Subproceso** lanza `program.exe --container <uuid> --ipc-port <puerto>`
3. **program.exe** escucha en `127.0.0.1:<puerto>`
4. **Python** envía `CMD_MOUNT` con el perfil de sesión
5. **program.exe** responde `{1}` y queda listo para IOCTLs

---

## USO EN TIEMPO DE EJECUCIÓN

### Escenario Normal (Sin VAL 5)

```
[Cliente VALORANT] 
       │
       │ Named Pipe: \\.\pipe\933823D3-...
       ▼
[vClient.exe en PC Gaming]
       │
       │ Túnel TLS :51820
       ▼
[main.py en PC Servidor]
       │
       │ Session Manager
       ▼
[wine_manager.send_ioctl()]
       │
       │ Socket IPC 127.0.0.1:5928
       ▼
[program.exe HandleIOCTL()]
       │
       │ CryptoContext.GenerateAccessResponse()
       │ - Deriva clave de JWT
       │ - Calcula HMAC-SHA256(input + key)
       │ - Mezcla con contador y timestamp
       │ - Devuelve 293 bytes (header Protobuf + payload)
       ▼
[Respuesta asciende por la cadena]
       │
       ▼
[Cliente VALORANT] ← Verificación exitosa, NO hay VAL 5
```

### Escenario Fallback (Si program.exe falla)

Si `program.exe` no está disponible o falla:
1. `wine_manager.py` detecta el fallo
2. Cambia a `vgc_crypto.py` (modo STUB)
3. Usa `FALLBACK_TOKEN` con ruido aleatorio
4. **Riesgo**: Mayor probabilidad de VAL 5 en sesiones largas

---

## RESPUESTAS CRIPROGRÁFICAS

### IOCTL 0x22C03C (ACCESS)

Genera un blob de **293 bytes**:
- **Bytes 0-19**: Header Protobuf fijo
  ```
  08 01 12 A0 02 52 47 01 00 05 FA A7 74 C9 93 69 50 77 F4 B0
  ```
- **Bytes 20-280**: Payload dinámico
  ```python
  payload[i] = HMAC_SHA256(key, input)[i % 32] ^ (heartbeat_count & 0xFF)
  ```

### IOCTL 0x222000 (HEARTBEAT_STUB)

Similar al ACCESS pero optimizado para latencia ultra-baja:
- Basado en timestamp nanosegundos
- Contador interno incremental
- XOR con clave de sesión

---

## SOLUCIÓN DE PROBLEMAS

### program.exe no inicia

**Síntoma**: Logs muestran "program.exe ready timeout"

**Causas**:
1. Puerto ya en uso
2. Ruta incorrecta
3. Permisos insuficientes

**Solución**:
```cmd
REM Verificar puerto
netstat -ano | findstr :5928

REM Verificar archivo
dir server\program.exe

REM Ejecutar manualmente para debug
server\program.exe --container test123 --ipc-port 5928 --work-dir data
```

### Respuestas lentas (>100ms)

**Causa**: CPU saturada o disco lento

**Solución**:
- Mover `work_dir` a SSD
- Reducir `max_clients` en config.yaml
- Verificar que no haya antivirus escaneando

### VAL 5 persiste

**Verificar**:
1. ✅ `wine.enabled: true` en config.yaml
2. ✅ `program.exe` existe en `server/program.exe`
3. ✅ Logs muestran "program.exe mounted"
4. ✅ No hay errores de timeout en IPC

**Posible causa raíz**:
- Sesiones duplicadas (mismo PUUID en múltiples contenedores)
- JWT expirado antes de entrar a queue
- Firewall bloqueando IPC local

---

## SEGURIDAD Y STEALTH

### Medidas Anti-Detección

1. **Claves por sesión**: Cada JWT genera una clave AES única
2. **Ruido temporal**: Timestamps mezclados en cada respuesta
3. **Contadores únicos**: No hay dos heartbeats idénticos
4. **Sin patrones estáticos**: Eliminado XOR fijo sobre tokens

### Lo que NO hace program.exe

❌ No modifica memoria de VALORANT  
❌ No inyecta DLLs  
❌ No intercepta tráfico de red directamente  
❌ No altera archivos del juego  

Solo emula las respuestas criptográficas que un driver legítimo proporcionaría.

---

## REFERENCIAS TÉCNICAS

### Archivos Relacionados

| Archivo | Función |
|---------|---------|
| `server/program.cpp` | Código fuente de program.exe |
| `server/program_worker.py` | Cliente Python que habla con program.exe |
| `server/wine_manager.py` | Gestiona ciclo de vida de contenedores |
| `server/vgc_crypto.py` | Fallback criptográfico en Python |
| `server/vgc_driver.py` | Lógica de IOCTLs del driver |

### Estructura de Directorios

```
amulator/
├── server/
│   ├── program.cpp        ← Fuente C++
│   ├── program.exe        ← Binario compilado (DEBE EXISTIR)
│   ├── program_worker.py  ← Cliente IPC
│   ├── wine_manager.py    ← Gestor de contenedores
│   ├── main.py            ← Servidor TLS principal
│   └── ...
├── config.yaml            ← Configuración (wine.enabled: true)
└── data/
    └── containers/        ← Work dir para contenedores
```

---

## CONCLUSIÓN

`program.exe` es el componente **CRÍTICO** para eliminar el Error VAL 5 porque:

1. **Proporciona entropía real**: Respuestas únicas e impredecibles
2. **Mantiene compatibilidad**: Headers Protobuf idénticos a vgk.sys real
3. **Opera en modo usuario**: Sin necesidad de driver kernel real
4. **Baja latencia**: <5ms por IOCTL via socket local

**Sin program.exe → Modo STUB → Posible VAL 5**  
**Con program.exe → Modo Real → Sesiones estables**
