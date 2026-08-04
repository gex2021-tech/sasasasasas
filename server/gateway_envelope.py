import hashlib
import hmac
import os
import struct
import time
from typing import Dict, Any

from .vgc_crypto import FALLBACK_TOKEN
from .vgc_tokens import build_f1_token, build_f15_token


def _encode_varint(value: int) -> bytes:
    """Encode integer as protobuf varint"""
    buf = bytearray()
    while value > 0x7F:
        buf.append((value & 0x7F) | 0x80)
        value >>= 7
    buf.append(value & 0x7F)
    return bytes(buf)


def _encode_protobuf_field(field_num: int, wire_type: int, data: bytes) -> bytes:
    """Encode protobuf field with tag"""
    tag = (field_num << 3) | wire_type
    result = bytearray(_encode_varint(tag))
    if wire_type == 2:  # length-delimited
        result.extend(_encode_varint(len(data)))
    result.extend(data)
    return bytes(result)


def build_gateway_envelope(
    session_id: str,
    hwid_hex: str,
    puuid: str,
    region: str,
    build_info: Dict[str, Any],
    rsa_spki_pem: bytes,
    timestamp_ms: int
) -> bytes:
    """Builds a Vanguard gateway protobuf envelope with freshly signed F1 and F15 tokens."""
    envelope = bytearray()
    
    # Field 1: Protocol version = 1
    envelope.extend(_encode_protobuf_field(1, 0, _encode_varint(1)))
    
    # Field 2: F1 Token (proper 6-component structure with valid HMAC signatures)
    try:
        hwid_bytes = bytes.fromhex(hwid_hex) if hwid_hex else os.urandom(32)
    except ValueError:
        hwid_bytes = os.urandom(32)
        
    f1_token = build_f1_token(puuid, hwid_bytes, timestamp_ms)
    envelope.extend(_encode_protobuf_field(2, 2, f1_token))
    
    # Field 15: F15 Token (Base64 SHA1 hash of F1 + version + fixed_suffix)
    vanguard_version = "1.18.3.74"
    f15_token = build_f15_token(f1_token, vanguard_version)
    envelope.extend(_encode_protobuf_field(15, 2, f15_token.encode('utf-8')))
    
    return bytes(envelope)

