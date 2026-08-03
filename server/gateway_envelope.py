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
    """Builds a Vanguard gateway protobuf envelope with session-specific data.
    
    Structure:
    - Field 1: version (varint) = 1
    - Field 2: signed_token (bytes) - F1 token with proper structure
    - Field 3: client_info (embedded message)
    - Field 4: timestamp (fixed64)
    - Field 5: os_info (embedded message) - CRITICAL FOR VAL 5 FIX
    - Field 15: f15_token (string) - SHA1-based validation token
    """
    envelope = bytearray()
    
    # Field 1: Protocol version
    envelope.extend(_encode_protobuf_field(1, 0, b'\x01'))
    
    # Field 2: F1 Token (proper 6-component structure)
    try:
        hwid_bytes = bytes.fromhex(hwid_hex) if hwid_hex else os.urandom(32)
    except ValueError:
        hwid_bytes = os.urandom(32)
    f1_token = build_f1_token(puuid, hwid_bytes, timestamp_ms)
    envelope.extend(_encode_protobuf_field(2, 2, f1_token))
    
    # Field 3: Client info (embedded message)
    client_info = bytearray()
    
    # Subfield 1: region
    client_info.extend(_encode_protobuf_field(1, 2, region.encode('utf-8')))
    
    # Subfield 2: puuid
    if puuid:
        client_info.extend(_encode_protobuf_field(2, 2, puuid.encode('utf-8')))
    
    # Subfield 3: build version
    build_str = f"{build_info.get('major', 13)}.{build_info.get('minor', 2)}.{build_info.get('patch', 0)}"
    client_info.extend(_encode_protobuf_field(3, 2, build_str.encode('utf-8')))
    
    # Subfield 4: build changelist
    changelist = build_info.get('changelist', 5092570)
    client_info.extend(_encode_protobuf_field(4, 0, _encode_varint(changelist)))
    
    # Subfield 5: RSA public key (if provided)
    if rsa_spki_pem:
        # Extract DER from PEM for compact representation
        pem_lines = rsa_spki_pem.split(b'\n')
        der_b64 = b''.join(line.strip() for line in pem_lines 
                          if not line.startswith(b'-----'))
        client_info.extend(_encode_protobuf_field(5, 2, der_b64[:128]))  # Truncate for size
    
    envelope.extend(_encode_protobuf_field(3, 2, bytes(client_info)))
    
    # Field 4: Server timestamp
    envelope.extend(_encode_protobuf_field(4, 1, struct.pack('<Q', timestamp_ms)))
    
    # Field 5: OS Info (CRITICAL - VAL 5 FIX)
    # Community research: must send platform=1, arch=2, version=10.0.19045
    # variant=6 triggers VAL 5 in queue!
    os_info = bytearray()
    
    # Subfield 1: platform (varint) - 1=Windows
    os_info.extend(_encode_protobuf_field(1, 0, b'\x01'))
    
    # Subfield 2: architecture (varint) - 2=x64
    os_info.extend(_encode_protobuf_field(2, 0, b'\x02'))
    
    # Subfield 3: version (string) - "10.0.19045" (Windows 10 21H2)
    os_info.extend(_encode_protobuf_field(3, 2, b'10.0.19045'))
    
    # Subfield 4: variant (varint) - 1=Pro (NOT 6, that triggers VAL 5!)
    os_info.extend(_encode_protobuf_field(4, 0, b'\x01'))
    
    envelope.extend(_encode_protobuf_field(5, 2, bytes(os_info)))
    
    # Field 15: F15 Token (Base64 SHA1 hash of F1 + version + suffix)
    client_version = f"{build_info.get('major', 1)}.{build_info.get('minor', 18)}.{build_info.get('patch', 3)}.74"
    f15_token = build_f15_token(f1_token, client_version)
    envelope.extend(_encode_protobuf_field(15, 2, f15_token.encode('utf-8')))
    
    return bytes(envelope)

