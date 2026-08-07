# INFORME TÉCNICO DE AUDITORÍA Y DIAGNÓSTICO: ERRORES PERSISTENTES VAL 5

**Fecha**: 2026-08-06  
**Módulo objetivo**: `amulator` (VGC Emulator 2-PC Architecture)  
**Estado**: Fallo persistente en fase de búsqueda de partida (Queue / Matchmaking). 5 desconexiones consecutivas por **VAL 5**.

---

## 1. RESUMEN EJECUTIVO

A pesar de haber aplicado la separación de caché dual (`g_cached_hb_payload` vs `g_cached_driver_status`), la codificación de Protobuf en orden ascendente (`variant=1` en `OSInfo`), y la autenticación HTTPS directa con Vanguard Gateway, **el sistema continúa registrando el error VAL 5 tras 2-4 minutos en cola**.

Las herramientas de diagnóstico automáticas muestran que la capa básica de protocolo (handshake TLS, formato de tokens F1/F15 y respuestas IOCTL estáticas) es correcta. Sin embargo, los logs revelan **44 a 87 reconexiones de sesión por minuto (session churn)** y saltos severos en los tiempos de latido, lo que demuestra que el módulo de estado de Vanguard en `vgc.exe` rechaza la sesión durante el chequeo dinámico de emparejamiento.

---

## 2. HALLAZGOS Y EVIDENCIA TÉCNICA (LOGS & ANALYZER)

### A. Churn Masivo de Sesiones (`FREQUENT_RECONNECTS`)
* **Evidencia**: `logs/server.log` muestra repetidos mensajes `purging old duplicate session ... for puuid=b5e70706` (entre 64 y 87 purgas por ciclo).
* **Causa**: El cliente `vClient.exe` está sufriendo desconexiones de socket TLS o reenviando solicitudes `SESSION_AUTH` completas en lugar de mantener la conexión persistente. Esto destruye la instancia criptográfica (`CryptoSession`) y fuerza la regeneración del sobre de Gateway en la VPS, provocando una desincronización de tokens con el cliente oficial de Riot.

### B. Violaciones de Tiempo en Latidos (`TIMING_VIOLATION`)
* **Evidencia**: Los intervalos entre latidos registrados en los logs muestran deltas de `326s`, `520s` e incluso saltos masivos.
* **Causa**: El bucle `_loop()` en `session_manager.py` o la transmisión Named Pipe en `vClient.cpp` experimenta bloqueos I/O síncronos al procesar el fallback de criptografía o peticiones HTTPS. Cuando la brecha entre latidos supera los **15 segundos**, Vanguard marca el token de latido como expirado y destruye el estado del driver (`VAL 5`).

---

## 3. VECTOR DE FALLO NO RESUELTO (ANÁLISIS DE CÓDIGO FUENTE)

Para el equipo de desarrollo/código, los siguientes 4 puntos son los **bloqueantes reales** que causan el VAL 5 en producción:

### 1. Rotación Dinámica de JWT / Token Decay en Cola
* **Problema**: Durante la búsqueda de partida, Riot Client renueva el `entitlements_token` y el `id_token`. Si `vClient` no intercepta el mensaje de actualización de token en las pipes `0x65` o `0x10` y lo transmite vía `MsgType.JWT_UPDATE` al servidor, Vanguard Gateway invalida el sobre en caché (`b"RG..."`) a los 2 minutos.
* **Solución requerida**: Implementar un listener activo de actualización de JWT en `vClient.cpp` que fuerce un `JWT_UPDATE` inmediato sin reiniciar la sesión TLS completa.

### 2. Validación de Hashes Polinomiales en `IOCTL 0x22200C` (Memory Scan) / `0x222004` (Integrity)
* **Problema**: El manejador actual en `vgc_driver.py` responde con hashes estáticos (`b"VALORANT_CLEAN_STATE"`). En parches recientes, `vgc.exe` envía un reto de escaneo de memoria donde exige calcular un hash polinomial dinámico usando las constantes `POS_MULS` y `NEG_MULS` extraídas de los módulos del juego en ejecución.
* **Solución requerida**: Actualizar `_integrity_check()` y `_memory_scan()` para procesar la semilla del buffer de entrada (`input_data`) e incluir la respuesta de verificación de hash del cliente.

### 3. Falta de Multiplexación Criptográfica en `vClient.cpp`
* **Problema**: Cuando `vClient` recibe un ping en las pipes `0x03` o `0x04`, lee `g_cached_hb_payload`. Si la VPS tardó más de 10 segundos en actualizar esa variable global por latencia de red, `vClient` responde a `vgc.exe` con un latido obsoleto (replay token).
* **Solución requerida**: Agregar una estampa de tiempo local en `vClient.cpp` para validar que `g_cached_hb_payload` tenga menos de 8 segundos de antigüedad antes de entregarlo a la pipe. Si está obsoleto, generar el paquete provisional con el nonce local.

### 4. SMBIOS / TPM Attestation Mismatch (`IOCTL 0x222008`)
* **Problema**: `vgc_driver.py` emula la respuesta de atestiguamiento (`_attestation`) generando un HMAC estático sobre la clave de sesión y el tiempo. En la fase de entrada a partida, Vanguard valida la consistencia entre el HWID enviado en `SESSION_AUTH` y la estructura del chip TPM / SecureBoot.
* **Solución requerida**: Enlazar los valores del pool de máquinas (`machine_pool.py`) directamente con los campos codificados en el protobuf de atestiguamiento (BootID, firmware hashes).

---

## 4. CHECKLIST ACCIONABLE PARA EL PROGRAMADOR

```markdown
[ ] 1. vClient.cpp: Eliminar la reconexión TLS completa en caídas menores; usar paquete MsgType.SYNC (3) en lugar de reinstanciar SESSION_AUTH.
[ ] 2. vClient.cpp: Agregar timestamp de expiración a g_cached_hb_payload (máximo 8s de vida útil en caché local).
[ ] 3. session_manager.py: Convertir el bucle _loop() a un hilo dedicado por sesión (Threaded Scheduler) para evitar que una sesión lenta bloquee a las demás.
[ ] 4. gateway_envelope.py: Asegurar que el refresco de JWT pida el token real a Riot API o vClient en lugar de reutilizar el token inicial del login.
[ ] 5. vgc_driver.py: Actualizar el cálculo del reto IOCTL 0x222008 (Attestation) para incluir los valores de SecureBoot/TPM de la máquina seleccionada en machine_pool.
```

---

## 5. CONCLUSIÓN

Los arreglos automáticos locales ajustaron correctamente la configuración (`config.yaml`), pero el problema principal reside en **la desincronización de reconexión de `vClient` (87 purgas de sesión)** y en la **obsolescencia del token de latido en caché**. Pasando este informe al desarrollador C++/Python, se podrá implementar la persistencia de socket en `vClient` y la actualización activa de JWTs requerida para superar la comprobación de cola de Vanguard.
