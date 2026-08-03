"""Protocolo tunnel client.dll <-> servidor (TLS TCP 51820)."""
from enum import IntEnum
import struct
from typing import Tuple

from dataclasses import dataclass

HEADER = struct.Struct("!II")
MAX_PAYLOAD = 4 * 1024 * 1024
IOCTL_VGK = 0x222000


class MsgType(IntEnum):
    HELLO = 1
    HELLO_OK = 2
    SYNC = 3
    IOCTL = 4
    IOCTL_RESP = 5
    HEARTBEAT_BUFFER = 6
    PING = 7
    PONG = 8
    ERROR = 9
    JWT_UPDATE = 10
    JWT_OK = 11
    PIPE_AUTH = 12
    PIPE_AUTH_OK = 13
    SESSION_AUTH = 14
    SESSION_AUTH_OK = 15
    TASK_RESULT = 16  # NEW - response to heartbeat tasks


def pack(msg_type: int, payload: bytes = b"") -> bytes:
    if len(payload) > MAX_PAYLOAD:
        raise ValueError("payload too large")
    return HEADER.pack(msg_type, len(payload)) + payload


def unpack_header(data: bytes) -> Tuple[int, int]:
    if len(data) < HEADER.size:
        raise ValueError("short header")
    return HEADER.unpack(data[:HEADER.size])


def pack_hello(auth_key: str, client_hwid: bytes) -> bytes:
    key = auth_key.encode("utf-8")
    body = struct.pack("!I", len(key)) + key + struct.pack("!I", len(client_hwid)) + client_hwid
    return pack(MsgType.HELLO, body)


def parse_hello(payload: bytes) -> Tuple[str, bytes]:
    off = 0
    klen = struct.unpack_from("!I", payload, off)[0]
    off += 4
    key = payload[off:off + klen].decode("utf-8")
    off += klen
    hlen = struct.unpack_from("!I", payload, off)[0]
    off += 4
    return key, payload[off:off + hlen]


def pack_hello_ok(session_id: str) -> bytes:
    sid = session_id.encode("utf-8")
    return pack(MsgType.HELLO_OK, struct.pack("!I", len(sid)) + sid)


def pack_sync(session_id: str, last_sequence: int) -> bytes:
    sid = session_id.encode("utf-8")
    body = struct.pack("!I", len(sid)) + sid + struct.pack("!Q", last_sequence)
    return pack(MsgType.SYNC, body)


def parse_sync(payload: bytes) -> Tuple[str, int]:
    off = 0
    slen = struct.unpack_from("!I", payload, off)[0]
    off += 4
    sid = payload[off:off + slen].decode("utf-8")
    off += slen
    return sid, struct.unpack_from("!Q", payload, off)[0]


def pack_ioctl(ioctl_code: int, data: bytes) -> bytes:
    body = struct.pack("!I", ioctl_code) + struct.pack("!I", len(data)) + data
    return pack(MsgType.IOCTL, body)


def parse_ioctl(payload: bytes) -> Tuple[int, bytes]:
    ioctl_code = struct.unpack_from("!I", payload, 0)[0]
    dlen = struct.unpack_from("!I", payload, 4)[0]
    return ioctl_code, payload[8:8 + dlen]


def pack_ioctl_resp(data: bytes) -> bytes:
    return pack(MsgType.IOCTL_RESP, struct.pack("!I", len(data)) + data)


def pack_heartbeat_buffer(sequence: int, data: bytes) -> bytes:
    body = struct.pack("!Q", sequence) + struct.pack("!I", len(data)) + data
    return pack(MsgType.HEARTBEAT_BUFFER, body)


def pack_pong() -> bytes:
    return pack(MsgType.PONG)


def parse_jwt_update(payload: bytes) -> Tuple[str, str]:
    off = 0
    jlen = struct.unpack_from("!I", payload, off)[0]
    off += 4
    jwt = payload[off:off + jlen].decode("utf-8")
    off += jlen
    plen = struct.unpack_from("!I", payload, off)[0]
    off += 4
    puuid = payload[off:off + plen].decode("utf-8")
    return jwt, puuid


def pack_jwt_ok() -> bytes:
    return pack(MsgType.JWT_OK)


def pack_pipe_auth_ok() -> bytes:
    return pack(MsgType.PIPE_AUTH_OK)


def parse_pipe_auth(payload: bytes) -> int:
    if len(payload) < 4:
        return 0
    return struct.unpack_from("!I", payload, 0)[0]


def _parse_len_prefixed(payload: bytes, off: int) -> Tuple[bytes, int]:
    slen = struct.unpack_from("!I", payload, off)[0]
    off += 4
    data = payload[off:off + slen]
    off += slen
    return data, off


@dataclass
class SessionAuthData:
    auth_key: str
    gateway_machine_id: bytes
    jwt: str
    puuid: str
    valorant_pid: int
    client_ts_ms: int = 0
    region: str = ""
    hwid_fingerprint: bytes = b""
    riot_account: str = ""
    hostname: str = ""
    rsa_spki_pem: bytes = b""
    build_branch: str = ""
    build_changelist: int = 0
    build_major: int = 0
    build_minor: int = 0
    build_patch: int = 0
    build_flags: int = 0
    external_sid: str = ""
    cpu_brand: str = ""
    cpu_model: str = ""
    gpu_brand: str = ""
    gpu_model: str = ""
    cpu_logical_count: int = 0


def _parse_len_prefixed_str(payload: bytes, off: int) -> Tuple[str, int]:
    data, off = _parse_len_prefixed(payload, off)
    return data.decode("utf-8"), off


def parse_session_auth(payload: bytes) -> SessionAuthData:
    off = 0
    auth_key, off = _parse_len_prefixed_str(payload, off)
    gateway_machine_id, off = _parse_len_prefixed(payload, off)
    jwt, off = _parse_len_prefixed_str(payload, off)
    puuid, off = _parse_len_prefixed_str(payload, off)
    valorant_pid = struct.unpack_from("!I", payload, off)[0] if off + 4 <= len(payload) else 0
    off += 4

    client_ts_ms = 0
    region = ""
    hwid_fingerprint = b""
    riot_account = ""
    hostname = ""
    rsa_spki_pem = b""
    build_branch = ""
    build_changelist = 0
    build_major = 0
    build_minor = 0
    build_patch = 0
    build_flags = 0
    external_sid = ""
    cpu_brand = ""
    cpu_model = ""
    gpu_brand = ""
    gpu_model = ""
    cpu_logical_count = 0

    if off + 8 <= len(payload):
        client_ts_ms = struct.unpack_from("!Q", payload, off)[0]
        off += 8
    if off < len(payload):
        region, off = _parse_len_prefixed_str(payload, off)
    if off < len(payload):
        hwid_fingerprint, off = _parse_len_prefixed(payload, off)
    if off < len(payload):
        riot_account, off = _parse_len_prefixed_str(payload, off)
    if off < len(payload):
        hostname, off = _parse_len_prefixed_str(payload, off)
    
    # Extended fields
    if off < len(payload):
        rsa_spki_pem, off = _parse_len_prefixed(payload, off)
    if off < len(payload):
        build_branch, off = _parse_len_prefixed_str(payload, off)
    if off + 4 <= len(payload):
        build_changelist = struct.unpack_from("!I", payload, off)[0]; off += 4
    if off + 4 <= len(payload):
        build_major = struct.unpack_from("!I", payload, off)[0]; off += 4
    if off + 4 <= len(payload):
        build_minor = struct.unpack_from("!I", payload, off)[0]; off += 4
    if off + 4 <= len(payload):
        build_patch = struct.unpack_from("!I", payload, off)[0]; off += 4
    if off + 4 <= len(payload):
        build_flags = struct.unpack_from("!I", payload, off)[0]; off += 4
    if off < len(payload):
        external_sid, off = _parse_len_prefixed_str(payload, off)
    if off < len(payload):
        cpu_brand, off = _parse_len_prefixed_str(payload, off)
    if off < len(payload):
        cpu_model, off = _parse_len_prefixed_str(payload, off)
    if off < len(payload):
        gpu_brand, off = _parse_len_prefixed_str(payload, off)
    if off < len(payload):
        gpu_model, off = _parse_len_prefixed_str(payload, off)
    if off + 4 <= len(payload):
        cpu_logical_count = struct.unpack_from("!I", payload, off)[0]; off += 4

    return SessionAuthData(
        auth_key=auth_key,
        gateway_machine_id=gateway_machine_id,
        jwt=jwt,
        puuid=puuid,
        valorant_pid=valorant_pid,
        client_ts_ms=client_ts_ms,
        region=region,
        hwid_fingerprint=hwid_fingerprint,
        riot_account=riot_account,
        hostname=hostname,
        rsa_spki_pem=rsa_spki_pem,
        build_branch=build_branch,
        build_changelist=build_changelist,
        build_major=build_major,
        build_minor=build_minor,
        build_patch=build_patch,
        build_flags=build_flags,
        external_sid=external_sid,
        cpu_brand=cpu_brand,
        cpu_model=cpu_model,
        gpu_brand=gpu_brand,
        gpu_model=gpu_model,
        cpu_logical_count=cpu_logical_count,
    )


def pack_session_auth_ok(session_id: str, gateway_envelope: bytes = b"") -> bytes:
    sid = session_id.encode("utf-8")
    body = struct.pack("!I", len(sid)) + sid
    body += struct.pack("!I", len(gateway_envelope)) + gateway_envelope
    return pack(MsgType.SESSION_AUTH_OK, body)


def pack_task_result(task_result_data: bytes) -> bytes:
    """Pack TaskResultRequest message (type 16)"""
    return pack(MsgType.TASK_RESULT, task_result_data)

