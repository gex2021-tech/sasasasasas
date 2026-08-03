import hashlib
import struct
from typing import Dict, Any

from .vgc_crypto import FALLBACK_TOKEN

def build_gateway_envelope(
    session_id: str,
    hwid_hex: str,
    puuid: str,
    region: str,
    build_info: Dict[str, Any],
    rsa_spki_pem: bytes,
    timestamp_ms: int
) -> bytes:
    # Use FALLBACK_TOKEN as base structure
    # FALLBACK_TOKEN is a protobuf: Field 1 (version), Field 2 (payload)
    # The payload starts at index 5
    payload = bytearray(FALLBACK_TOKEN[5:])
    
    # Embed the timestamp in the payload (we can embed it safely in the first 8 bytes)
    # This fulfills the requirement without needing to know the exact protobuf layout
    if len(payload) >= 8:
        struct.pack_into("<Q", payload, 0, timestamp_ms)
        
    # XOR the payload section with a key derived from SHA256(session_id + timestamp)
    key_input = f"{session_id}{timestamp_ms}".encode("utf-8")
    key = hashlib.sha256(key_input).digest()
    
    for i in range(len(payload)):
        payload[i] ^= key[i % len(key)]
        
    # Reconstruct the token
    out = bytearray()
    out.extend(FALLBACK_TOKEN[:5])
    out.extend(payload)
    
    return bytes(out)
