# VGC Emulator — Guía Rápida y Estructura Limpia

---

## 🎯 Archivos Principales (Lo único que necesitas usar)

### 🖥️ **1. CLIENTE (Tu PC Gaming)**
* **[`emulator_loader.py`](emulator_loader.py)** ⭐️ -> **EL LOADER INTERACTIVO**. Este es el ejecutable GUI principal que abre la ventana con la barra de progreso, hace el bypass de VGC y envía la autenticación.
  ```powershell
  python emulator_loader.py
  ```

---

### 🌐 **2. SERVIDOR (Tu VM / VPS / PC Backend)**
* **[`server/main.py`](server/main.py)** ⭐️ -> **EL SERVIDOR PRINCIPAL**. Escucha en `0.0.0.0:51820`, genera el pool de 500 máquinas y gestiona `program.exe` en segundo plano.
  ```bash
  python -m server.main
  ```

---

### ⚙️ **3. CONFIGURACIÓN**
* **[`config.yaml`](config.yaml)** ⭐️ -> Archivo de configuración central de IPs, puertos y llaves de cifrado.

---

### 🛠️ **4. SCRIPTS DE SETUP ÚTILES**
* **[`setup_after_vanguard.bat`](setup_after_vanguard.bat)** -> Script automático post-instalación de Vanguard.
* **[`test_client.py`](test_client.py)** -> Script para verificar la conexión TLS entre cliente y servidor.

---

## 📂 Organización de la Carpeta

* `docs/` -> Todos los documentos de análisis, guías y notas de parches (`.md`).
* `server/` -> Código fuente del servidor Python y ejecutable worker (`program.exe`).
* `certs/` -> Certificados TLS.
* `data/` -> Caché de sesiones y estado del loader.
* `logs/` -> Registros de eventos de sesiones.
