"""VGC Driver Emulation - IOCTL Response Generator

Simulates vgk.sys driver responses with realistic VGC behavior.
Includes integrity checks, memory scans, and attestation payloads.
"""
from __future__ import annotations

import hashlib
import hmac
import logging
import random
import struct
import time
from dataclasses import dataclass, field
from typing import Dict, Optional

log = logging.getLogger("vgc_driver")

# IOCTL codes from vgk.sys
IOCTL_VGK_HEARTBEAT = 0x222000
IOCTL_VGK_ACCESS = 0x22C03C
IOCTL_VGK_INTEGRITY = 0x222004
IOCTL_VGK_ATTESTATION = 0x222008
IOCTL_VGK_MEMORY_SCAN = 0x22200C
IOCTL_VGK_MODULE_CHECK = 0x222010
IOCTL_VGK_DRIVER_STATUS = 0x22C0EC  # Critical - reports vgk.sys state to vgc.exe


@dataclass
class DriverState:
    """Persistent state for VGC driver emulation"""
    session_id: str
    boot_time: float = field(default_factory=time.time)
    scan_count: int = 0
    last_scan: float = 0.0
    integrity_failures: int = 0
    suspicious_modules: list = field(default_factory=list)
    
    # Attestation state
    last_attestation: float = 0.0
    attestation_nonce: bytes = b""
    
    # Memory scan state
    last_memory_scan: float = 0.0
    memory_scan_regions: int = 0


class VGCDriver:
    """Emulates vgk.sys driver IOCTL responses"""
    
    def __init__(self):
        self.states: Dict[str, DriverState] = {}
    
    def get_state(self, session_id: str) -> DriverState:
        """Get or create driver state for session"""
        if session_id not in self.states:
            self.states[session_id] = DriverState(session_id=session_id)
        return self.states[session_id]
    
    def handle_ioctl(
        self,
        session_id: str,
        ioctl_code: int,
        input_data: bytes,
        aes_key: bytes
    ) -> bytes:
        """Main IOCTL dispatcher"""
        state = self.get_state(session_id)
        
        handlers = {
            IOCTL_VGK_HEARTBEAT: self._heartbeat,
            IOCTL_VGK_ACCESS: self._access_check,
            IOCTL_VGK_INTEGRITY: self._integrity_check,
            IOCTL_VGK_ATTESTATION: self._attestation,
            IOCTL_VGK_MEMORY_SCAN: self._memory_scan,
            IOCTL_VGK_MODULE_CHECK: self._module_check,
            IOCTL_VGK_DRIVER_STATUS: self._driver_status,
        }
        
        handler = handlers.get(ioctl_code)
        if handler:
            return handler(state, input_data, aes_key)
        
        # Generic response for unknown ioctls
        return self._generic_response(ioctl_code, input_data, aes_key)
    
    def _heartbeat(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x222000 - Heartbeat with system state"""
        state.scan_count += 1
        state.last_scan = time.time()
        
        # Build protobuf-like response
        # Field 1: version (varint)
        # Field 2: status (varint) - 0=clean, 1=suspicious, 2=detected
        # Field 3: timestamp (fixed64)
        # Field 4: scan_count (varint)
        # Field 5: signature (bytes)
        # Field 6: windows_security_flags (varint) - NEW
        
        response = bytearray()
        
        # Version = 1
        response.extend(self._encode_protobuf_field(1, 0, b'\x01'))
        
        # Status = 0 (clean)
        status = 0
        if state.integrity_failures > 3:
            status = 1  # suspicious
        if state.integrity_failures > 10:
            status = 2  # detected
        response.extend(self._encode_protobuf_field(2, 0, bytes([status])))
        
        # Timestamp
        ts = int(time.time() * 1000)
        response.extend(self._encode_protobuf_field(3, 1, struct.pack("<Q", ts)))
        
        # Scan count
        response.extend(self._encode_protobuf_field(4, 0, self._encode_varint(state.scan_count)))
        
        # Generate HMAC signature
        sig_data = bytes(response)
        signature = hmac.new(key, sig_data, hashlib.sha256).digest()
        response.extend(self._encode_protobuf_field(5, 2, signature))
        
        # Windows Security Flags (NEW - critical for VAL 5)
        # Bits: 0=HVCI, 1=IOMMU, 2=SecureBoot, 3=VBS, 4=TPM2
        security_flags = 0b11111  # All enabled (0x1F)
        response.extend(self._encode_protobuf_field(6, 0, self._encode_varint(security_flags)))
        
        # Add noise to prevent static detection
        import os
        noise_bytes = os.urandom(random.randint(8, 32))
        response.extend(self._encode_protobuf_field(99, 2, noise_bytes))
        
        log.debug(
            "heartbeat session=%s scan=%d status=%d len=%d flags=0x%X",
            state.session_id[:8],
            state.scan_count,
            status,
            len(response),
            security_flags
        )
        
        return bytes(response)
    
    def _access_check(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x22C03C - Access permission check"""
        # If input data provided, it's a specific permission check
        if data:
            # Parse permission request
            # For now, always grant
            return b"GRANTED\x00"
        
        # General access check - return system status
        return b"CLEAN\x00"
    
    def _integrity_check(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x222004 - Integrity verification"""
        # Simulate integrity check of game files
        # In real VGC, this checks PE headers, code sections, etc.
        
        response = bytearray()
        
        # Field 1: check_type (varint)
        response.extend(self._encode_protobuf_field(1, 0, b'\x01'))  # Full check
        
        # Field 2: result (varint) - 0=pass, 1=suspicious, 2=fail
        result = 0
        if random.random() < 0.01:  # 1% chance of "suspicious"
            result = 1
            state.integrity_failures += 1
        response.extend(self._encode_protobuf_field(2, 0, bytes([result])))
        
        # Field 3: regions_checked (varint)
        regions = random.randint(50, 150)
        response.extend(self._encode_protobuf_field(3, 0, self._encode_varint(regions)))
        
        # Field 4: hash (bytes) - SHA256 of "clean" state
        clean_hash = hashlib.sha256(b"VALORANT_CLEAN_STATE").digest()
        response.extend(self._encode_protobuf_field(4, 2, clean_hash))
        
        log.debug(
            "integrity session=%s result=%d regions=%d",
            state.session_id[:8],
            result,
            regions
        )
        
        return bytes(response)
    
    def _attestation(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x222008 - Cryptographic attestation"""
        state.last_attestation = time.time()
        
        # Parse challenge from input (if provided)
        challenge = data if data else random.randbytes(32)
        state.attestation_nonce = challenge
        
        # Build attestation response
        response = bytearray()
        
        # Field 1: challenge_response (bytes)
        # HMAC(key, challenge || session_id || timestamp)
        attest_data = challenge + state.session_id.encode() + struct.pack("<d", time.time())
        attest_sig = hmac.new(key, attest_data, hashlib.sha256).digest()
        response.extend(self._encode_protobuf_field(1, 2, attest_sig))
        
        # Field 2: boot_id (varint) - unique per boot
        boot_id = int(state.boot_time * 1000) & 0xFFFFFFFF
        response.extend(self._encode_protobuf_field(2, 0, self._encode_varint(boot_id)))
        
        # Field 3: driver_version (string)
        version = b"vgk.sys 1.18.3-74+20260623.212037"
        response.extend(self._encode_protobuf_field(3, 2, version))
        
        log.debug(
            "attestation session=%s nonce=%s",
            state.session_id[:8],
            challenge[:8].hex()
        )
        
        return bytes(response)
    
    def _memory_scan(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x22200C - Memory region scan"""
        state.last_memory_scan = time.time()
        state.memory_scan_regions += 1
        
        response = bytearray()
        
        # Field 1: scan_type (varint) - 1=quick, 2=deep
        scan_type = 1 if len(data) < 16 else 2
        response.extend(self._encode_protobuf_field(1, 0, bytes([scan_type])))
        
        # Field 2: regions_scanned (varint)
        regions = random.randint(100, 500) if scan_type == 2 else random.randint(20, 50)
        response.extend(self._encode_protobuf_field(2, 0, self._encode_varint(regions)))
        
        # Field 3: anomalies_found (varint)
        anomalies = 0  # Always clean
        response.extend(self._encode_protobuf_field(3, 0, bytes([anomalies])))
        
        # Field 4: scan_duration_ms (varint)
        duration = random.randint(50, 200)
        response.extend(self._encode_protobuf_field(4, 0, self._encode_varint(duration)))
        
        log.debug(
            "memory_scan session=%s type=%d regions=%d",
            state.session_id[:8],
            scan_type,
            regions
        )
        
        return bytes(response)
    
    def _module_check(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x222010 - Loaded module verification"""
        response = bytearray()
        
        # Field 1: modules_checked (varint)
        modules = random.randint(30, 80)
        response.extend(self._encode_protobuf_field(1, 0, self._encode_varint(modules)))
        
        # Field 2: suspicious_count (varint)
        suspicious = len(state.suspicious_modules)
        response.extend(self._encode_protobuf_field(2, 0, bytes([suspicious])))
        
        # Field 3: result (varint) - 0=pass, 1=suspicious, 2=detected
        result = 0
        if suspicious > 0:
            result = 1
        if suspicious > 5:
            result = 2
        response.extend(self._encode_protobuf_field(3, 0, bytes([result])))
        
        log.debug(
            "module_check session=%s modules=%d suspicious=%d",
            state.session_id[:8],
            modules,
            suspicious
        )
        
        return bytes(response)
    
    def _driver_status(self, state: DriverState, data: bytes, key: bytes) -> bytes:
        """IOCTL 0x22C0EC - Driver status report (vgk.sys state)
        
        Critical IOCTL that vgc.exe uses to verify driver is loaded and functioning.
        Missing this causes VAL 5 when queueing.
        """
        response = bytearray()
        
        # Field 1: driver_loaded (varint) - 1=loaded
        response.extend(self._encode_protobuf_field(1, 0, b'\x01'))
        
        # Field 2: driver_version (string)
        driver_ver = b"vgk.sys 1.18.3-74+20260623.212037"
        response.extend(self._encode_protobuf_field(2, 2, driver_ver))
        
        # Field 3: boot_time (fixed64)
        boot_ms = int(state.boot_time * 1000)
        response.extend(self._encode_protobuf_field(3, 1, struct.pack("<Q", boot_ms)))
        
        # Field 4: protection_enabled (varint) - 1=enabled
        response.extend(self._encode_protobuf_field(4, 0, b'\x01'))
        
        # Field 5: windows_security_features (varint)
        # Bits: 0=HVCI, 1=IOMMU, 2=SecureBoot, 3=VBS, 4=TPM2
        sec_features = 0b11111  # All enabled
        response.extend(self._encode_protobuf_field(5, 0, bytes([sec_features])))
        
        # Field 6: kernel_integrity_level (varint) - 0-100
        integrity = 100  # Perfect
        response.extend(self._encode_protobuf_field(6, 0, bytes([integrity])))
        
        # Field 7: signature (bytes) - HMAC of response
        sig_data = bytes(response)
        signature = hmac.new(key, sig_data, hashlib.sha256).digest()
        response.extend(self._encode_protobuf_field(7, 2, signature))
        
        log.debug(
            "driver_status session=%s loaded=1 integrity=%d",
            state.session_id[:8],
            integrity
        )
        
        return bytes(response)
    
    def _generic_response(self, ioctl_code: int, data: bytes, key: bytes) -> bytes:
        """Fallback for unknown IOCTL codes"""
        # Return basic success response
        response = bytearray()
        response.extend(self._encode_protobuf_field(1, 0, b'\x00'))  # status=success
        response.extend(self._encode_protobuf_field(2, 2, data if data else b'OK'))
        
        log.debug("generic_ioctl code=0x%X len=%d", ioctl_code, len(data))
        
        return bytes(response)
    
    # Protobuf encoding helpers
    
    def _encode_varint(self, value: int) -> bytes:
        """Encode integer as protobuf varint"""
        buf = bytearray()
        while value > 0x7F:
            buf.append((value & 0x7F) | 0x80)
            value >>= 7
        buf.append(value & 0x7F)
        return bytes(buf)
    
    def _encode_protobuf_field(self, field_num: int, wire_type: int, data: bytes) -> bytes:
        """Encode protobuf field: (field_num << 3) | wire_type + data
        
        Wire types:
        0 = varint
        1 = fixed64
        2 = length-delimited
        5 = fixed32
        """
        tag = (field_num << 3) | wire_type
        result = bytearray(self._encode_varint(tag))
        
        if wire_type == 2:  # length-delimited
            result.extend(self._encode_varint(len(data)))
        
        result.extend(data)
        return bytes(result)


# Global driver instance
_driver = VGCDriver()


def handle_driver_ioctl(
    session_id: str,
    ioctl_code: int,
    input_data: bytes,
    aes_key: bytes
) -> bytes:
    """Global entry point for driver IOCTL handling"""
    return _driver.handle_ioctl(session_id, ioctl_code, input_data, aes_key)
