"""In-process vgk crypto session — mount + IOCTL payload generation.

Stealth features:
  - Per-heartbeat token noise via HMAC-SHA256 rolling key
  - Session-bound AES key derivation using HMAC(jwt, hwid)
  - Protobuf header preserved while payload bytes vary
"""
from __future__ import annotations

import hashlib
import hmac
import json
import logging
import struct
import time
from dataclasses import dataclass, field
from typing import Any, Dict, Optional

log = logging.getLogger("vgc_crypto")

# From DndVanguardV2 vgc_state.hpp FALLBACK_TOKEN (gateway HB payload).
FALLBACK_TOKEN = bytes([
    0x08, 0x01, 0x12, 0xA0, 0x02, 0x52, 0x47, 0x01, 0x00, 0x05, 0xFA, 0xA7,
    0x74, 0xC9, 0x93, 0x69, 0x50, 0x77, 0xF4, 0xB0, 0xD9, 0xC8, 0x0D, 0x6F,
    0x67, 0x57, 0x08, 0xCB, 0xFC, 0x03, 0x06, 0x60, 0x70, 0x2C, 0x73, 0x9E,
    0x2C, 0xA5, 0xF7, 0x25, 0xF0, 0x4E, 0x2A, 0x8F, 0x9F, 0xB5, 0xC7, 0x06,
    0xA9, 0x4E, 0x78, 0x15, 0x7B, 0x20, 0x7D, 0xD3, 0x0F, 0xC5, 0xB8, 0x24,
    0xEE, 0xD2, 0xBC, 0xA1, 0x9E, 0x83, 0x0F, 0x34, 0x98, 0x2F, 0x3D, 0xED,
    0xF1, 0x3A, 0xD2, 0x63, 0xDC, 0xA0, 0xA6, 0x16, 0x9F, 0xAA, 0x21, 0xD5,
    0xA4, 0xE9, 0x1C, 0xFE, 0xB6, 0x7A, 0xC2, 0x4B, 0x0C, 0x6F, 0x90, 0x7B,
    0x6F, 0x80, 0x77, 0x70, 0x67, 0x3B, 0x0A, 0xB5, 0x2A, 0x4A, 0x71, 0xBF,
    0xBE, 0xE9, 0xBE, 0x4C, 0xBE, 0xF3, 0xC2, 0xBE, 0xCD, 0x2F, 0xB2, 0xDA,
    0xE8, 0x82, 0xDB, 0xDD, 0x3F, 0xF0, 0x5A, 0x98, 0x0D, 0xA0, 0x2D, 0x7F,
    0xAD, 0xDA, 0xE7, 0xD6, 0xF5, 0x9D, 0x32, 0x1D, 0x0B, 0x38, 0x48, 0x9F,
    0x03, 0xBD, 0x23, 0xF0, 0x39, 0x76, 0x52, 0x67, 0x8F, 0x02, 0x32, 0x3B,
    0xBC, 0x82, 0xCA, 0x10, 0xDE, 0x6A, 0xC7, 0x3C, 0x51, 0x14, 0xFF, 0x58,
    0x8B, 0xFE, 0x7B, 0x63, 0xA6, 0xE2, 0x9D, 0xDB, 0x5B, 0xC0, 0xCD, 0x7F,
    0x92, 0xCE, 0xA6, 0x5D, 0x0C, 0x19, 0x25, 0x00, 0x6E, 0xDC, 0x7B, 0x3B,
    0x0F, 0x68, 0x2B, 0xE1, 0xDD, 0xE8, 0x66, 0x03, 0x70, 0x58, 0x3E, 0x5F,
    0xEA, 0xB1, 0x65, 0x68, 0x4C, 0xB1, 0x2D, 0xF9, 0x7E, 0xD9, 0x45, 0xBF,
    0x06, 0xAD, 0xDF, 0x74, 0xFC, 0x1A, 0x5F, 0x09, 0x41, 0x33, 0xA6, 0x30,
    0xF2, 0xD6, 0x02, 0xE6, 0xCB, 0x46, 0x37, 0xF3, 0x2B, 0x7A, 0xB9, 0x7A,
    0xC6, 0x06, 0x13, 0x7C, 0x0A, 0xF5, 0x78, 0xB4, 0x36, 0x43, 0xDD, 0x6E,
    0xBF, 0x68, 0xBF, 0x90, 0xC7, 0x0E, 0x7D, 0x19, 0x72, 0xBB, 0xDA, 0x9F,
    0xF5, 0x44, 0x82, 0x96, 0x2F, 0xD0, 0x2F, 0xEB, 0x49, 0xBE, 0x8B, 0x17,
    0x05, 0x5D, 0xE3, 0x8C, 0x10, 0xBA, 0xB3, 0x42, 0x7C, 0x01, 0xDD, 0xA9,
    0x00, 0xE5, 0xC2, 0x6D, 0xD0,
])

IOCTL_ACCESS = 0x22C03C
IOCTL_HEARTBEAT_STUB = 0x222000

# Protobuf header occupies bytes 0..19 — never touch these.
_NOISE_START = 20
_NOISE_END = 280


# ---------------------------------------------------------------------------
#  Stealth helpers
# ---------------------------------------------------------------------------

def _derive_session_key(jwt: str, hwid_hex: str) -> bytes:
    """HMAC-SHA256 key derivation binding JWT to hardware identity.

    Stronger than plain SHA256(jwt) — ties the crypto session to a
    specific machine fingerprint so replayed JWTs from different HWIDs
    produce different key material.
    """
    if not hwid_hex:
        return hashlib.sha256(jwt.encode("utf-8")).digest()
    return hmac.new(
        jwt.encode("utf-8"),
        hwid_hex.encode("utf-8"),
        hashlib.sha256,
    ).digest()


def _noise_token(
    base: bytes,
    session_id: str,
    sequence: int,
    timestamp: float,
) -> bytes:
    """Apply per-heartbeat noise to the FALLBACK_TOKEN.

    Preserves the protobuf header (bytes 0..19) intact. XORs bytes
    20..280 with a rolling HMAC key derived from (session_id, seq, ts).
    Each heartbeat produces a unique payload — defeats static
    signature detection while keeping the outer protobuf envelope valid.
    """
    # Build a rolling key: HMAC-SHA256(session_id, seq || ts)
    msg = struct.pack("!Qd", sequence, timestamp)
    rolling = hmac.new(
        session_id.encode("utf-8") if session_id else b"fallback",
        msg,
        hashlib.sha256,
    ).digest()

    out = bytearray(base)
    end = min(_NOISE_END, len(out))
    key_len = len(rolling)

    for i in range(_NOISE_START, end):
        out[i] ^= rolling[(i - _NOISE_START) % key_len]

    return bytes(out)


# ---------------------------------------------------------------------------
#  CryptoSession
# ---------------------------------------------------------------------------

@dataclass
class CryptoSession:
    profile: Dict[str, Any] = field(default_factory=dict)
    aes_key: bytes = b""
    mounted: bool = False
    hb_count: int = 0
    last_mount_at: float = 0.0

    def mount(self, profile: Dict[str, Any]) -> None:
        self.profile = dict(profile)
        jwt = str(profile.get("jwt", ""))
        hwid_hex = str(profile.get("hwid_fingerprint_hex", ""))
        if jwt:
            self.aes_key = _derive_session_key(jwt, hwid_hex)
        else:
            seed = json.dumps(profile, sort_keys=True, default=str).encode("utf-8")
            self.aes_key = hashlib.sha256(seed).digest()
        self.mounted = True
        self.last_mount_at = time.time()
        log.info(
            "crypto mounted session=%s puuid=%s key=%s",
            str(profile.get("session_id", ""))[:8],
            str(profile.get("client_puuid", ""))[:8],
            self.aes_key[:4].hex(),
        )

    def update_jwt(self, jwt: str, puuid: str) -> None:
        self.profile["jwt"] = jwt
        self.profile["client_puuid"] = puuid
        if jwt:
            hwid_hex = str(self.profile.get("hwid_fingerprint_hex", ""))
            self.aes_key = _derive_session_key(jwt, hwid_hex)

    def heartbeat_payload(self) -> bytes:
        self.hb_count += 1
        if self.profile.get("gateway_token"):
            return bytes(self.profile["gateway_token"])
        # Apply per-heartbeat noise instead of returning static token
        session_id = str(self.profile.get("session_id", ""))
        return _noise_token(FALLBACK_TOKEN, session_id, self.hb_count, time.time())

    def ioctl_response(self, ioctl_code: int, data: bytes) -> bytes:
        if not self.mounted:
            return b""

        # Import driver here to avoid circular dependency
        from .vgc_driver import handle_driver_ioctl
        
        session_id = str(self.profile.get("session_id", ""))
        
        # Use enhanced driver for realistic responses
        return handle_driver_ioctl(
            session_id=session_id,
            ioctl_code=ioctl_code,
            input_data=data,
            aes_key=self.aes_key
        )


def profile_to_mount_json(profile: Dict[str, Any]) -> bytes:
    safe = {
        k: (v.hex() if isinstance(v, (bytes, bytearray)) else v)
        for k, v in profile.items()
    }
    return json.dumps(safe, separators=(",", ":")).encode("utf-8")
