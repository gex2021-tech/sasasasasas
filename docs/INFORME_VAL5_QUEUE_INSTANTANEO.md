# INFORME TÉCNICO DEFINITIVO: VAL 5 INSTANTÁNEO AL PRESIONAR QUEUE

**Fecha**: 2026-08-07  
**Síntoma**: VAL 5 aparece **inmediatamente** al tocar el botón de Queue o Partida Personalizada. El jugador puede permanecer en el menú principal sin problemas.  
**Implicación**: El fallo NO es de heartbeat timing ni de token decay. Es un **rechazo instantáneo de Vanguard Gateway** durante el chequeo de "readiness" que ocurre al momento de entrar en cola.

---

## 1. ANÁLISIS DE LA CAUSA RAÍZ

Al presionar Queue, el cliente de VALORANT envía una solicitud de matchmaking a los servidores de Riot. **Antes de aceptar al jugador en cola**, Riot verifica con Vanguard que:

1. El **Gateway Envelope** enviado durante `SESSION_AUTH` sea válido y verificable.
2. Los tokens de autenticación (`entitlements_token`, `id_token`) sean tokens reales de Riot, no fabricados localmente.
3. La firma del sobre sea verificable contra la infraestructura de Riot.

El emulador **falla en los 3 puntos**. A continuación el análisis de cada uno.

---

## 2. FALLO #1: TOKENS FABRICADOS LOCALMENTE (CRÍTICO)

### Evidencia en el código:

En [`gateway_envelope.py` L83-121](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/server/gateway_envelope.py#L83-L121), las funciones `_fetch_entitlement()` y `_fetch_id_token()` **fabrican JWTs falsos localmente** en lugar de usar los tokens reales de Riot:

```python
# gateway_envelope.py L83-99 — FABRICACIÓN LOCAL
def _fetch_entitlement(self, jwt: str) -> str:
    """Fallback local entitlement token builder"""   # ← "Fallback" que SIEMPRE se ejecuta
    header = {"typ": "JWT", "alg": "HS256"}
    payload = {
        "sub": f"riot_entitlement_{int(time.time())}",  # ← Inventado
        "iss": "riot-entitlements",                      # ← Inventado
        "features": ["vanguard_auth", "game_session"],   # ← Inventado
    }
    # Firma con SHA256 local, NO con la clave privada de Riot
    signature = hashlib.sha256(f"{header_b64}.{payload_b64}.{jwt}".encode()).digest()
```

### Por qué causa VAL 5 instantáneo:
Riot Gateway valida la firma del `entitlements_token` con su clave pública RSA. Un JWT firmado con `SHA256(string)` local es **criptográficamente inválido** y se rechaza en milisegundos.

### Corrección necesaria:
El `entitlements_token` y el `id_token` deben ser **los tokens reales** que el Riot Client genera durante el login. `vClient.exe` debe capturarlos del proceso de Riot y transmitirlos vía `SESSION_AUTH` al backend.

---

## 3. FALLO #2: GATEWAY SIEMPRE DEVUELVE 200 (FALLBACK SILENCIOSO)

### Evidencia en el código:

En [`gateway_envelope.py` L271-284](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/server/gateway_envelope.py#L271-L284), `post_gateway_auth()` **nunca reporta un fallo real**:

```python
# L275-277 — Si Riot devuelve HTTP 403/401/400, el código lo IGNORA y usa fallback
elif res:
    log.warning(f"[GW] Gateway HTTP {res.status_code}: {res.text[:150]}")
    return 200, build_gateway_auth_response(session_id, region)  # ← Devuelve 200 FALSO

# L281-284 — Si la conexión falla, TAMBIÉN devuelve 200 falso
response_body = build_gateway_auth_response(session_id, region)
return 200, response_body  # ← Siempre "éxito"
```

Y el `build_gateway_auth_response()` genera un JSON genérico que Vanguard no reconoce:

```python
# L287-299 — Respuesta fabricada
def build_gateway_auth_response(session_id, region):
    response = {
        "status": "authenticated",    # ← String inventado
        "action_next": 5,             # ← Número inventado
        "magic": 0x66,                # ← Valor inventado
    }
    return json.dumps(response).encode('utf-8')  # ← JSON, no Protobuf
```

### Por qué causa VAL 5:
Vanguard Gateway responde con un **blob Protobuf binario**, no con JSON. El servidor guarda este JSON falso como `gateway_response`, y cuando `vgc.exe` lo verifica al entrar en cola, lo rechaza instantáneamente.

### Corrección necesaria:
`post_gateway_auth()` debe:
1. **Propagar el error real** si Riot rechaza la autenticación (no hacer fallback silencioso).
2. **No devolver 200** si la respuesta no es HTTP 200 real.
3. Log del status code real para diagnóstico.

---

## 4. FALLO #3: ENVOLVENTE PROTOBUF NO COINCIDE CON EL ESQUEMA DE RIOT

### Evidencia en el código:

En [`gateway_envelope.py` L320-394](file:///c:/Users/gex20/OneDrive/Escritorio/amulator/server/gateway_envelope.py#L320-L394), `build_gateway_envelope()` construye un Protobuf con un esquema **asumido**, no el esquema real de Riot:

```python
# Campos actuales del envelope:
Field 1: version (varint=1)
Field 2: F1 token (fabricado con HMAC local)
Field 3: entitlements_token (puede ser fabricado)
Field 4: id_token (puede ser fabricado)
Field 5: F15 token (derivado del F1 fabricado)
Field 6: client_info sub-message
Field 7: timestamp (fixed64)
Field 8: OS info sub-message
Field 15: F15 token duplicado
```

### Problemas específicos:
- **F1 y F15 tokens** son generados por `vgc_tokens.py` usando HMAC-SHA256 local. El Gateway real espera tokens firmados con la clave del driver `vgk.sys`.
- **Field 15 duplica Field 5**: El F15 token aparece dos veces (campos 5 y 15). Esto puede confundir el parser del Gateway.
- **El esquema de campos no ha sido validado** contra una captura real de tráfico de un sistema limpio (sin emulador).

---

## 5. CHECKLIST PARA EL DESARROLLADOR

```
[ ] 1. TOKENS REALES: vClient.exe debe capturar entitlements_token e id_token
       del proceso de Riot Client (RiotClientServices.exe) y enviarlos
       en el payload de SESSION_AUTH. NO fabricar localmente.

[ ] 2. GATEWAY HONESTO: post_gateway_auth() debe devolver el status code
       REAL de Riot. Si devuelve 403 o 401, propagarlo como fallo.
       Eliminar el fallback silencioso build_gateway_auth_response().

[ ] 3. ESQUEMA PROTOBUF: Capturar el tráfico real del gateway desde un
       sistema limpio (sin emulador, con Vanguard real) usando Wireshark
       o mitmproxy. Comparar el esquema de campos con el que genera
       build_gateway_envelope(). Corregir los campos que no coincidan.

[ ] 4. F1/F15 TOKENS: Verificar si vgk.sys real genera estos tokens con
       una clave de hardware o una clave de sesión del driver. Si es así,
       el emulador necesita replicar esa derivación de claves exacta.

[ ] 5. LOGS DE DIAGNÓSTICO: Agregar logging del status code real y body
       de la respuesta del Gateway para poder depurar rechazos futuros:
       log.error("Gateway REAL status=%d body=%s", res.status_code, res.text[:200])
```

---

## 6. RESUMEN EJECUTIVO

| Componente | Estado | Causa |
|---|---|---|
| Menú Principal | ✅ Funciona | No requiere validación de Gateway |
| Queue / Partida | ❌ VAL 5 instantáneo | Gateway rechaza tokens fabricados |
| `entitlements_token` | ❌ Fabricado localmente | Firma JWT inválida |
| `id_token` | ❌ Fabricado localmente | Firma JWT inválida |
| `post_gateway_auth()` | ❌ Fallback silencioso | Siempre devuelve 200 aunque falle |
| `build_gateway_envelope()` | ⚠️ Esquema no verificado | Puede tener campos incorrectos |

**Conclusión**: El VAL 5 al presionar Queue es causado porque el servidor envía tokens JWT fabricados localmente a Riot Gateway, que los rechaza instantáneamente. La función `post_gateway_auth()` oculta este rechazo devolviendo un falso HTTP 200 con un JSON genérico. Para resolver esto, `vClient.exe` debe capturar y transmitir los tokens reales del Riot Client, y `post_gateway_auth()` debe propagar los errores reales del Gateway.
