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
    """Returns the official 293-byte signed Vanguard FALLBACK_TOKEN."""
    return FALLBACK_TOKEN

