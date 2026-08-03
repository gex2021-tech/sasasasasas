"""Heartbeat Task Processing - Decrypt and Respond to Server Tasks

VAL 5 Fix: The server sends encrypted tasks (293 byte blob) in heartbeat responses.
These must be decrypted with HKDF + AES-GCM and answered via TaskResultRequest (type 11).
"""
from __future__ import annotations

import hashlib
import hmac
import logging
import struct
import time
from typing import Optional, Tuple
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.hazmat.primitives.kdf.hkdf import HKDF
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.backends import default_backend

log = logging.getLogger("heartbeat_tasks")


def hkdf_extract(ikm: bytes, salt: bytes) -> bytes:
    """HKDF Extract phase - derive PRK from input key material"""
    return hmac.new(salt, ikm, hashlib.sha256).digest()


def hkdf_expand(prk: bytes, info: bytes, length: int = 32) -> bytes:
    """HKDF Expand phase - derive output key from PRK"""
    hkdf = HKDF(
        algorithm=hashes.SHA256(),
        length=length,
        salt=None,
        info=info,
        backend=default_backend()
    )
    # HKDF.expand expects the PRK as input
    return hkdf.derive(prk)


def decrypt_heartbeat_blob(
    encrypted_blob: bytes,
    request_aes_key: bytes,
    salt: bytes = b"vanguard_hb_salt"
) -> Optional[bytes]:
    """Decrypt 293-byte heartbeat task blob
    
    Process:
    1. HKDF_Extract(request_aes_key, salt) -> PRK
    2. HKDF_Expand(PRK, "heartbeat_encryption", 32) -> AES key
    3. AES-GCM decrypt with key
    
    Args:
        encrypted_blob: 293 byte encrypted payload from server
        request_aes_key: Session AES key used for heartbeat request
        salt: HKDF salt (default from reverse engineering)
    
    Returns:
        Decrypted task data or None on failure
    """
    try:
        # Step 1: HKDF Extract
        prk = hkdf_extract(request_aes_key, salt)
        
        # Step 2: HKDF Expand
        info = b"heartbeat_encryption"
        aes_key = hkdf_expand(prk, info, length=32)
        
        # Step 3: AES-GCM Decrypt
        # Blob structure: [nonce: 12 bytes][ciphertext: N bytes][tag: 16 bytes]
        if len(encrypted_blob) < 28:  # 12 + 16 minimum
            log.error("heartbeat blob too short: %d bytes", len(encrypted_blob))
            return None
        
        nonce = encrypted_blob[:12]
        ciphertext_and_tag = encrypted_blob[12:]
        
        aesgcm = AESGCM(aes_key)
        plaintext = aesgcm.decrypt(nonce, ciphertext_and_tag, None)
        
        log.debug("decrypted heartbeat tasks: %d bytes", len(plaintext))
        return plaintext
        
    except Exception as e:
        log.error("failed to decrypt heartbeat blob: %s", e)
        return None


def parse_tasks(plaintext: bytes) -> list[dict]:
    """Parse decrypted task list
    
    Task structure (protobuf):
    - Field 1: task_id (varint)
    - Field 2: task_type (varint)
    - Field 3: task_data (bytes)
    
    Returns:
        List of task dicts with id, type, data
    """
    tasks = []
    offset = 0
    
    while offset < len(plaintext):
        try:
            # Read field tag
            tag, offset = _read_varint(plaintext, offset)
            field_num = tag >> 3
            wire_type = tag & 0x7
            
            if field_num == 1:  # task_id
                task_id, offset = _read_varint(plaintext, offset)
                tasks.append({"id": task_id, "type": 0, "data": b""})
            
            elif field_num == 2:  # task_type
                task_type, offset = _read_varint(plaintext, offset)
                if tasks:
                    tasks[-1]["type"] = task_type
            
            elif field_num == 3:  # task_data
                if wire_type == 2:  # length-delimited
                    length, offset = _read_varint(plaintext, offset)
                    data = plaintext[offset:offset + length]
                    offset += length
                    if tasks:
                        tasks[-1]["data"] = data
            
            else:
                # Unknown field, skip
                if wire_type == 0:  # varint
                    _, offset = _read_varint(plaintext, offset)
                elif wire_type == 2:  # length-delimited
                    length, offset = _read_varint(plaintext, offset)
                    offset += length
                else:
                    break
        
        except Exception as e:
            log.warning("error parsing tasks at offset %d: %s", offset, e)
            break
    
    log.info("parsed %d tasks from heartbeat", len(tasks))
    return tasks


def build_task_results(tasks: list[dict], session_key: bytes) -> bytes:
    """Build TaskResultRequest (message type 11) response
    
    Structure:
    - Field 1: task_count (varint)
    - Field 2: results (repeated embedded message)
      - Subfield 1: task_id (varint)
      - Subfield 2: status (varint) - 0=success
      - Subfield 3: result_data (bytes)
    - Field 3: signature (bytes) - HMAC-SHA256
    
    Returns:
        Serialized protobuf for TaskResultRequest
    """
    response = bytearray()
    
    # Field 1: task_count
    response.extend(_encode_protobuf_field(1, 0, _encode_varint(len(tasks))))
    
    # Field 2: results (repeated)
    for task in tasks:
        result_msg = bytearray()
        
        # Subfield 1: task_id
        result_msg.extend(_encode_protobuf_field(1, 0, _encode_varint(task["id"])))
        
        # Subfield 2: status (0=success)
        result_msg.extend(_encode_protobuf_field(2, 0, b'\x00'))
        
        # Subfield 3: result_data (fake success response)
        result_data = b"OK\x00" + struct.pack("<Q", int(time.time() * 1000))
        result_msg.extend(_encode_protobuf_field(3, 2, result_data))
        
        response.extend(_encode_protobuf_field(2, 2, bytes(result_msg)))
    
    # Field 3: HMAC signature
    signature = hmac.new(session_key, bytes(response), hashlib.sha256).digest()
    response.extend(_encode_protobuf_field(3, 2, signature))
    
    return bytes(response)


# Protobuf encoding/decoding helpers

def _read_varint(data: bytes, offset: int) -> Tuple[int, int]:
    """Read varint from bytes at offset, return (value, new_offset)"""
    value = 0
    shift = 0
    while offset < len(data):
        byte = data[offset]
        offset += 1
        value |= (byte & 0x7F) << shift
        if not (byte & 0x80):
            break
        shift += 7
    return value, offset


def _encode_varint(value: int) -> bytes:
    """Encode integer as protobuf varint"""
    buf = bytearray()
    while value > 0x7F:
        buf.append((value & 0x7F) | 0x80)
        value >>= 7
    buf.append(value & 0x7F)
    return bytes(buf)


def _encode_protobuf_field(field_num: int, wire_type: int, data: bytes) -> bytes:
    """Encode protobuf field: (field_num << 3) | wire_type + data"""
    tag = (field_num << 3) | wire_type
    result = bytearray(_encode_varint(tag))
    
    if wire_type == 2:  # length-delimited
        result.extend(_encode_varint(len(data)))
    
    result.extend(data)
    return bytes(result)
