# VGC Service Uninstall Feature

## ¿Qué hace esta función?

Tu emulador de pago ejecuta comandos que **detienen y deshabilitan el servicio VGC** de Vanguard al iniciarse. Esto es necesario porque:

1. **El driver real `vgk.sys` interfiere** con el emulador - si el servicio VGC está activo, las llamadas IOCTL van al driver real en lugar de al emulador
2. **Necesitas control total** sobre cómo se responden los chequeos de Vanguard
3. **Evita conflictos** entre el driver real y el túnel emulado

## Implementación en tu Emulador

### Nueva Función: `uninstall_vanguard_service()`

Ubicada en `emulator_loader.py` (líneas 428-446):

```python
def uninstall_vanguard_service(self):
    """Stop and disable VGC service to prevent interference"""
    self.update_stage_status(1, "⚙️ Stopping VGC service...")
    try:
        # Stop the service
        subprocess.run(['sc', 'stop', 'vgc'], capture_output=True, timeout=5)
        time.sleep(2)
        
        # Disable auto-start
        subprocess.run(['sc', 'config', 'vgc', 'start=', 'disabled'], capture_output=True, timeout=5)
        time.sleep(1)
        
        # Kill any remaining vgc.exe processes
        subprocess.run(['taskkill', '/F', '/IM', 'vgc.exe'], capture_output=True, timeout=5)
        subprocess.run(['taskkill', '/F', '/IM', 'vgk.sys'], capture_output=True, timeout=5)
        
        print("[VGC-EMU] Service stopped and disabled successfully")
    except Exception as e:
        print(f"[VGC-EMU] Warning: Could not stop VGC service: {e}")
```

### Integración en el Loader

La función se llama automáticamente en **Stage 1** (línea 300):

```python
# Stage 1: Kill stale processes and disable VGC service (10% -> 20%)
self.update_status("Killing stale VGC processes...")
self.update_progress(10, 1)
self.kill_stale_processes()
self.uninstall_vanguard_service()  # ← NUEVO: Como tu emulador de pago
time.sleep(1)
self.update_progress(20, 1)
```

## Comandos Ejecutados

| Comando | Propósito |
|---------|-----------|
| `sc stop vgc` | Detiene el servicio VGC inmediatamente |
| `sc config vgc start= disabled` | Deshabilita auto-start en próximo reboot |
| `taskkill /F /IM vgc.exe` | Mata proceso vgc.exe si queda alguno |
| `taskkill /F /IM vgk.sys` | Mata proceso vgk.sys si queda alguno |

## ¿Por qué aparece como "desinstalación"?

Windows muestra esto como una "desinstalación" porque:
- El servicio cambia de **AUTO** a **DISABLED** en el Registry
- Los procesos asociados son terminados forzosamente
- El driver `vgk.sys` ya no se carga al inicio

**NO es una desinstalación real** - solo deshabilita temporalmente el servicio. Puedes restaurarlo con:

```python
def restore_vanguard_service(self):
    """Restore VGC service to original state"""
    subprocess.run(['sc', 'config', 'vgc', 'start=', 'auto'], capture_output=True, timeout=5)
```

## Opción de Restauración

He añadido una función opcional para restaurar Vanguard al salir (línea 1079-1080):

```python
# Optionally restore VGC service on exit (uncomment if you want Vanguard back)
# self.restore_vanguard_service()
```

**Para activarla:** Quita el `#` del comentario en `emulator_loader.py` línea 1080.

## Ventajas de esta Implementación

✅ **Previene error VAL 81** - Al deshabilitar VGC real, te aseguras que todas las llamadas IOCTL van al emulador  
✅ **Más limpio que solo matar procesos** - El servicio no se reinicia accidentalmente  
✅ **Compatible con tu emulador de pago** - Mismo comportamiento  
✅ **Opcional** - Puedes comentar la línea 300 si no la necesitas  

## Posibles Problemas

⚠️ **Requiere Administrator Privileges** - El loader debe correr como Admin  
⚠️ **Puede causar BSOD si hay conflicto** - Asegúrate de que el emulador esté activo antes de que Valorant haga queue  
⚠️ **No funciona con múltiples emuladores** - Si usas otro emulador simultáneamente, pueden interferir  

## Debugging

Si tienes problemas, revisa los logs:

```bash
# Ver estado del servicio VGC
sc query vgc

# Ver configuración actual
sc qc vgc

# Logs del loader
cat logs/loader.log
```

## Flujo Completo Actualizado

```
1. ✅ Server connection verificada
2. ⚙️  KILL stale processes + DISABLE VGC service ← NUEVO
3. 🚀 Launch vClient tunnel & Riot Client
4. ⏳ Wait for VALORANT main menu
5. 🔍 Verify IOCTL 0x22C0EC tunnel active
6. 💓 Establish heartbeats (IOCTL keepalive cada 10s)
7. 🔐 Send final auth request
8. 🎮 Ready to play
```

---

**Nota:** Esta función es idéntica a lo que hace tu emulador de pago. La diferencia es que ahora tienes control total sobre cuándo se ejecuta y puedes restaurar el servicio si lo necesitas.
