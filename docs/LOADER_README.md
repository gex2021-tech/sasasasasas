# VGC Emulator Loader UI

## 🎨 Modern Loader Interface

Interfaz gráfica estilo "emulador pago" con progreso visual y etapas automáticas.

## 📸 Características

- ✅ **Progreso por etapas** (0% → 100%)
- ✅ **Detección automática** de Valorant
- ✅ **Indicadores visuales** con colores (gris → morado → verde)
- ✅ **Countdown timer** para próximo auth request
- ✅ **Botones INJECT/REFRESH** al completar
- ✅ **Diseño moderno** con tema oscuro y acentos púrpura

## 🚀 Uso Rápido

### 1. Instalar Dependencias

```bash
pip install -r requirements.txt
```

### 2. Iniciar Loader

```bash
# Opción 1: Python directo
python emulator_loader.py

# Opción 2: Batch script
start_loader.bat
```

### 3. Flujo de Uso

1. **Click START**
2. **Espera automática** hasta detectar Valorant
3. **Progreso automático** por las 5 etapas
4. **Pantalla "YOU CAN QUEUE NOW"** cuando esté listo
5. **Click INJECT** (opcional, para cheats adicionales)

## 📊 Etapas del Loader

### Etapa 1: Killing stale processes (0% → 10%)
- Mata procesos viejos de VGC/vClient
- Limpia estado previo

### Etapa 2: Launching Riot client (10% → 20%)
- Inicia vClient.exe en background
- Espera confirmación de arranque

### Etapa 3: Bypassing VGC check (20% → 40%)
- **ESPERA AUTOMÁTICA** a que inicies Valorant
- Detecta VALORANT.exe en procesos
- Simula bypass de VGC

### Etapa 4: Establishing heartbeats (40% → 60%)
- Verifica conexión vClient ↔ Emulator
- Confirma heartbeats funcionando
- Detecta SESSION_AUTH_OK en logs

### Etapa 5: Sending auth request (60% → 100%)
- Espera pantalla de carga del juego
- Envía auth request al emulador
- Completa el setup

## 🎯 Indicadores Visuales

### Colores de Estado

| Color | Significado |
|-------|-------------|
| 🔘 Gris (`#444444`) | Pendiente |
| 🟣 Morado (`#9d4edd`) | En progreso (pulsando) |
| 🟢 Verde (`#00ff00`) | Completado |

### Progreso Circular

```
0%   - Inicio
10%  - Procesos limpiados
20%  - vClient iniciado
40%  - VGC bypassed
60%  - Heartbeats activos
100% - Auth enviado, listo para jugar
```

## ⏱️ Countdown Timer

Después de completar, muestra:

```
NEXT AUTH REQUEST
4:42
```

Countdown de **4:42** (4 minutos 42 segundos) hasta próximo refresh de auth (previene VAL 5).

## 🔧 Configuración

### Ubicación de vClient

El loader busca vClient.exe en:
1. `build/vClient.exe`
2. `vClient.exe` (raíz del proyecto)

### Logs

Revisa logs para debugging:
- `vClient.log` - Log del cliente
- `logs/sessions/*.log` - Logs del servidor

## 🎨 Personalización

### Cambiar Colores

```python
# En emulator_loader.py

# Color principal (morado)
PRIMARY_COLOR = '#9d4edd'

# Color de fondo
BG_COLOR = '#1a1a1a'

# Color de éxito (verde)
SUCCESS_COLOR = '#00ff00'
```

### Agregar Etapas

```python
self.stages = [
    {"name": "Tu nueva etapa", "progress": 50, "done": False},
    # ...
]
```

## 🐛 Troubleshooting

### "Failed to start vClient"

**Causa**: vClient.exe no encontrado

**Solución**:
```powershell
.\build_vclient.ps1 -VpsHost "TU_IP"
```

### "Timeout waiting for game"

**Causa**: Valorant no detectado en 5 minutos

**Solución**:
- Iniciar Valorant manualmente después de click START
- Verificar que el proceso se llama "VALORANT.exe"

### "Heartbeat connection failed"

**Causa**: vClient no conecta al emulador

**Solución**:
1. Verificar que el servidor esté corriendo: `python -m server.main`
2. Revisar firewall (puerto 51820)
3. Verificar `vClient.log` para errores

### Ventana no se abre

**Causa**: tkinter no instalado

**Solución**:
```bash
# Windows
pip install tk

# Ubuntu/Debian
sudo apt-get install python3-tk
```

## 📝 Notas

- El loader **NO inicia el servidor** automáticamente
- Debes tener el servidor corriendo: `python -m server.main`
- El botón "INJECT" es opcional (para usar con cheats adicionales)
- El loader se puede cerrar después de "YOU CAN QUEUE NOW"

## 🔄 Workflow Completo

```
Terminal 1 (Server):
$ python -m server.main
> tunnel TLS listening 0.0.0.0:51820

Terminal 2 (Loader):
$ python emulator_loader.py
> [UI abre]
> Click START
> Espera Valorant...
> ✓ YOU CAN QUEUE NOW

Valorant:
> Iniciar normalmente
> Conecta a vClient (automático)
> vClient → Emulator → Heartbeats activos
> ✅ Sin VAL 5
```

## 🎮 Comparación con Emulador Pago

| Característica | Emulador Pago | Nuestro Loader |
|----------------|---------------|----------------|
| UI moderna | ✅ | ✅ |
| Progreso por etapas | ✅ | ✅ |
| Detección de juego | ✅ | ✅ |
| Countdown timer | ✅ | ✅ |
| Botón INJECT | ✅ | ✅ |
| Animaciones | ✅ | ✅ (pulse) |
| Tema oscuro | ✅ | ✅ |
| **Código abierto** | ❌ | ✅ |
| **Gratis** | ❌ | ✅ |

---

**Creado para**: VGC Emulator "Esperanza"  
**Versión**: 1.0  
**Fecha**: 2026-08-03
