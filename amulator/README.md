# VGC Emulator "Esperanza"

Emulador de servidores de Valorant (VGC) para testing de cheats y desarrollo.

## Arquitectura Actual

El proyecto ha sido refactorizado a una arquitectura Python/C++:

- **Main Server (Python)**: El servidor principal se encuentra en el paquete `server/`. Maneja la lógica, sesiones y el protocolo binario sobre TLS.
- **vClient (C++)**: El cliente/tunnel (`server/main.cpp`) se ejecuta en el PC de juegos (Gaming PC).

## Estructura del Proyecto

```
amulator/
├── server/
│   ├── protocol.py           # Protocolo binario tunnel <-> servidor
│   ├── server.py             # Lógica del servidor principal en Python
│   └── main.cpp              # vClient para el Gaming PC
├── legacy/
│   └── server_prototype.cpp  # Prototipo original en C++
├── certs/                    # Claves y certificados TLS
├── config.yaml               # Configuración del servidor
├── test_client.py            # Herramienta de testing del protocolo
└── setup_gaming_pc.ps1       # Script de configuración para el Gaming PC
```
*(Nota: El directorio `src/` vacío y los archivos del prototipo antiguo se han movido a `legacy/` o eliminado).*

## Requisitos

- Windows 10/11
- Visual Studio 2022 con carga de trabajo C++ (para el vClient)
- Python 3.9+ (para el Main Server)

## 2-PC Setup Instructions

Para un entorno de pruebas seguro y realista, se requiere una configuración de 2 PCs:

### 1. Server PC (Python)
1. Instala los requerimientos para Python.
2. Asegúrate de tener los certificados TLS en la carpeta `certs/`.
3. Configura el servidor editando `config.yaml` o utilizando los valores por defecto.
4. Inicia el servidor de emulación:
   ```cmd
   python server/server.py
   ```

### 2. Gaming PC (vClient)
1. Ejecuta el script de configuración proporcionado para redirigir el tráfico VGC al Server PC. Asegúrate de ejecutarlo como Administrador:
   ```powershell
   .\setup_gaming_pc.ps1
   ```
   *El script añadirá entradas a tu archivo `hosts` y creará automáticamente un script `restore_hosts.ps1` para revertir los cambios cuando termines.*
2. Compila el cliente/tunnel (vClient) ubicado en `server/main.cpp`.
3. Ejecuta el vClient compilado en el Gaming PC.

## Testing

Puedes probar la comunicación con el servidor desde cualquier máquina utilizando el script de prueba provisto:
```cmd
python test_client.py --host <SERVER_IP> --port 51820
```
