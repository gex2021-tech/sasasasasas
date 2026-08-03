# VGC Emulator Loader - Visual Flow

## 🎬 Pantalla 1: Inicio

```
┌────────────────────────────────────────────────┐
│                                                │
│            VGC EMULATOR                        │
│            Esperanza v1.0                      │
│                                                │
│                   0%                           │
│         ┌──────────────────┐                   │
│         └──────────────────┘                   │
│                                                │
│         ● Killing stale processes              │
│         ● Launching Riot client                │
│         ● Bypassing VGC check                  │
│         ● Establishing heartbeats              │
│         ● Sending auth request                 │
│                                                │
│         ┌───────────────────┐                  │
│         │      START        │                  │
│         └───────────────────┘                  │
│                                                │
│               Exit                             │
│                                                │
│       Press START to begin                     │
└────────────────────────────────────────────────┘
```

## 🔄 Pantalla 2: Progreso 20% (Esperando Juego)

```
┌────────────────────────────────────────────────┐
│                                                │
│            VGC EMULATOR                        │
│            Esperanza v1.0                      │
│                                                │
│                  20%                           │
│         ┌█████─────────────┐                   │
│         └──────────────────┘                   │
│                                                │
│         ✓ Killing stale processes              │
│         ✓ Launching Riot client                │
│         ◉ Bypassing VGC check                  │ ← PULSANDO
│         ● Establishing heartbeats              │
│         ● Sending auth request                 │
│                                                │
│        [START button deshabilitado]            │
│                                                │
│               Exit                             │
│                                                │
│    ⏳ Waiting for VALORANT.exe...              │
└────────────────────────────────────────────────┘
```

## ⚡ Pantalla 3: Progreso 60% (Heartbeats)

```
┌────────────────────────────────────────────────┐
│                                                │
│            VGC EMULATOR                        │
│            Esperanza v1.0                      │
│                                                │
│                  60%                           │
│         ┌████████████──────┐                   │
│         └──────────────────┘                   │
│                                                │
│         ✓ Killing stale processes              │
│         ✓ Launching Riot client                │
│         ✓ Bypassing VGC check                  │
│         ◉ Establishing heartbeats              │ ← PULSANDO
│         ● Sending auth request                 │
│                                                │
│        [START button deshabilitado]            │
│                                                │
│               Exit                             │
│                                                │
│ Establishing heartbeats with server...         │
└────────────────────────────────────────────────┘
```

## ✅ Pantalla 4: Completado (100%)

```
┌────────────────────────────────────────────────┐
│                                                │
│                   ✓                            │
│                                                │
│         YOU CAN QUEUE NOW                      │
│                                                │
│                                                │
│   ┌──────────────┐    ┌──────────────┐        │
│   │   REFRESH    │    │    INJECT    │        │
│   └──────────────┘    └──────────────┘        │
│                                                │
│               Exit                             │
│                                                │
│          NEXT AUTH REQUEST                     │
│               4:42                             │
│                                                │
└────────────────────────────────────────────────┘
```

## 🎨 Código de Colores

### Estados de Progreso

```
Gris    ●  #444444  - Pendiente
Morado  ◉  #9d4edd  - En progreso (animado)
Verde   ✓  #00ff00  - Completado
```

### Paleta de Colores

```css
/* Fondo */
background: #1a1a1a (Negro suave)

/* Primario */
primary: #9d4edd (Morado vibrante)
primary-light: #c77dff (Morado claro)
primary-lighter: #e0aaff (Morado muy claro)

/* Éxito */
success: #00ff00 (Verde neón)

/* Texto */
text-primary: #ffffff (Blanco)
text-secondary: #999999 (Gris claro)
text-disabled: #666666 (Gris medio)
text-muted: #444444 (Gris oscuro)

/* Superficies */
surface: #2a2a2a (Gris muy oscuro)
surface-hover: #3a3a3a (Gris hover)
```

## ⏱️ Timeline de Animaciones

```
T+0s       : Pantalla inicial (START button)
           
T+0s       : [Usuario click START]

T+0-1s     : Stage 1: Killing processes
             Progress: 0% → 10%
             Indicator: ◉ Morado pulsando → ✓ Verde

T+1-3s     : Stage 2: Launching vClient
             Progress: 10% → 20%
             Indicator: ◉ Morado pulsando → ✓ Verde

T+3-?s     : Stage 3: Waiting for game
             Progress: 20% → 40%
             Indicator: ◉ Morado pulsando (esperando)
             Status: "⏳ Waiting for VALORANT.exe..."
             
[Usuario inicia Valorant]

T+?-?s     : Game detected!
             Progress: 30% → 40%
             Indicator: ✓ Verde
             Status: "Bypassing VGC check..."

T+?-?s     : Stage 4: Establishing heartbeats
             Progress: 40% → 60%
             Indicator: ◉ Morado pulsando → ✓ Verde
             Status: "Establishing heartbeats..."

T+?-?s     : Stage 5: Sending auth
             Progress: 60% → 80% → 100%
             Indicator: ◉ Morado pulsando → ✓ Verde
             Status: "Sending auth request..."

T+final    : SUCCESS SCREEN
             ✓ "YOU CAN QUEUE NOW"
             Buttons: REFRESH | INJECT
             Countdown: 4:42 → 4:41 → ...
```

## 🎯 Transiciones de Pantalla

### Transición 1→2 (Click START)
```python
# Deshabilitar botón START
start_button.config(state='disabled', bg='#444444')

# Iniciar thread de progreso
threading.Thread(target=emulator_sequence, daemon=True).start()
```

### Transición 2→3→4 (Progreso automático)
```python
# Update progress smoothly
for progress in range(20, 40):
    self.update_progress(progress, stage_index)
    time.sleep(0.1)  # Smooth animation
```

### Transición 4→Final (Success screen)
```python
# Clear all widgets
for widget in self.root.winfo_children():
    widget.destroy()

# Create new success UI
success_label = tk.Label(text="✓", fg='#00ff00')
ready_label = tk.Label(text="YOU CAN QUEUE NOW")
```

## 🔊 Estados de Mensaje

```python
# Mensajes de estado por etapa

Stage 0 (0-10%):
  "Killing stale VGC processes..."

Stage 1 (10-20%):
  "Starting vClient tunnel..."

Stage 2 (20-40%):
  "⏳ Waiting for VALORANT.exe..."  # Espera activa
  "Bypassing VGC check..."          # Después de detección

Stage 3 (40-60%):
  "Establishing heartbeats with server..."

Stage 4 (60-100%):
  "Waiting for game loading screen..."
  "Sending auth request..."

Success:
  "✓ Ready to play!"
```

## 🐛 Estados de Error

```python
# Si algo falla

Error en Stage 1:
  "❌ Failed to start vClient"
  [Botón START se reactiva]

Error en Stage 2:
  "❌ Timeout waiting for game"
  [Botón START se reactiva]

Error en Stage 3:
  "❌ Heartbeat connection failed"
  [Botón START se reactiva]

Error en Stage 4:
  "❌ Auth request failed"
  [Botón START se reactiva]
```

## 📱 Dimensiones

```
Window Size: 600x400 px
Centered: ✅

Title Font: Consolas, 24pt, Bold
Subtitle Font: Consolas, 10pt
Progress Font: Consolas, 36pt, Bold
Stage Font: Consolas, 11pt
Button Font: Consolas, 14pt, Bold
Status Font: Consolas, 9pt

Padding:
  - Title: 20px top
  - Progress: 30px top
  - Stages: 20px top
  - Buttons: 20px top
  - Status: 10px bottom
```

## 🎬 Demo Mode

Para probar sin Valorant:

```bash
python demo_loader.py
```

Simula automáticamente:
- ✅ vClient start
- ✅ Game detection (después de 5s)
- ✅ Heartbeats
- ✅ Auth request

**Flujo completo en ~15 segundos**

---

**Inspirado en**: Emuladores comerciales de VGC  
**Creado para**: VGC Emulator "Esperanza"  
**Tecnología**: Python + tkinter
