"""VGC Token Generation - F1 and F15 construction

Based on community research for VAL 5 fix:
- F1: 6 sub-tokens (nonce + 2x HMAC-SHA512 + empty + timestamp + hw_blob)
- F15: SHA1(F1 + client_version + suffix_16)
"""
from __future__ import annotations

import hashlib
import hmac
import os
import struct
import time
from base64 import b64encode
from typing import Tuple


def build_f1_token(puuid: str, hwid: bytes, timestamp_ms: int = None) -> bytes:
    """Build F1 token with 6 sub-components
    
    Structure:
    1. Nonce (16 bytes) - random
    2. HMAC-SHA512 #1 (64 bytes) - HMAC(secret_key, puuid + nonce)
    3. HMAC-SHA512 #2 (64 bytes) - HMAC(secret_key, hwid + timestamp)
    4. Empty token (0 bytes)
    5. Timestamp (6 bytes) - milliseconds since epoch, little-endian
    6. Hardware blob (variable) - derived from PUUID + HWID
    
    Args:
        puuid: Player UUID
        hwid: Hardware ID fingerprint (32 bytes)
        timestamp_ms: Optional timestamp in milliseconds
    
    Returns:
        Complete F1 token (typically ~170-200 bytes)
    """
    if timestamp_ms is None:
        timestamp_ms = int(time.time() * 1000)
    
    # Derive secret key from PUUID (first 32 bytes of SHA256)
    secret_key = hashlib.sha256(puuid.encode()).digest()
    
    # Component 1: Random nonce (16 bytes)
    nonce = os.urandom(16)
    
    # Component 2: HMAC-SHA512(secret_key, puuid + nonce)
    hmac_data_1 = puuid.encode() + nonce
    hmac_1 = hmac.new(secret_key, hmac_data_1, hashlib.sha512).digest()
    
    # Component 3: HMAC-SHA512(secret_key, hwid + timestamp)
    ts_bytes = struct.pack("<Q", timestamp_ms)
    hmac_data_2 = hwid[:32] + ts_bytes
    hmac_2 = hmac.new(secret_key, hmac_data_2, hashlib.sha512).digest()
    
    # Component 4: Empty (0 bytes)
    empty = b""
    
    # Component 5: Timestamp (6 bytes, little-endian)
    # Take lower 48 bits of timestamp
    ts_6bytes = struct.pack("<Q", timestamp_ms)[:6]
    
    # Component 6: Hardware blob derived from PUUID + HWID
    hw_blob_seed = (puuid + hwid.hex()).encode()
    hw_blob = hashlib.sha256(hw_blob_seed).digest()[:16]
    
    # Concatenate all components
    f1_token = nonce + hmac_1 + hmac_2 + empty + ts_6bytes + hw_blob
    
    return f1_token


def build_f15_token(f1_token: bytes, client_version: str) -> str:
    """Build F15 token from F1
    
    Structure:
    F15 = Base64(SHA1(F1 + client_version + fixed_suffix))
    
    Args:
        f1_token: Complete F1 token
        client_version: Client version string (e.g. "1.18.4.47")
    
    Returns:
        Base64-encoded F15 token
    """
    # Fixed suffix (16 bytes) - appears constant across clients
    fixed_suffix = b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f'
    
    # Combine F1 + version + suffix
    f15_data = f1_token + client_version.encode() + fixed_suffix
    
    # Hash with SHA1
    f15_hash = hashlib.sha1(f15_data).digest()
    
    # Encode to Base64
    f15_b64 = b64encode(f15_hash).decode('ascii')
    
    return f15_b64


def build_tokens(puuid: str, hwid: bytes, client_version: str) -> Tuple[bytes, str]:
    """Build both F1 and F15 tokens
    
    Args:
        puuid: Player UUID
        hwid: Hardware ID fingerprint
        client_version: Client version (e.g. "1.18.4.47")
    
    Returns:
        (f1_token, f15_token)
    """
    timestamp_ms = int(time.time() * 1000)
    f1 = build_f1_token(puuid, hwid, timestamp_ms)
    f15 = build_f15_token(f1, client_version)
    
    return f1, f15


def validate_f1_structure(f1_token: bytes) -> bool:
    """Validate F1 token has correct structure
    
    Expected minimum size: 16 + 64 + 64 + 0 + 6 + 16 = 166 bytes
    """
    if len(f1_token) < 166:
        return False
    
    # Check that HMAC portions look like random data (high entropy)
    hmac_1 = f1_token[16:80]
    hmac_2 = f1_token[80:144]
    
    # Simple entropy check - should have varied bytes
    if len(set(hmac_1)) < 30 or len(set(hmac_2)) < 30:
        return False
    
    return True
