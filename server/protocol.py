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
    )


def pack_session_auth_ok(session_id: str) -> bytes:
    sid = session_id.encode("utf-8")
    return pack(MsgType.SESSION_AUTH_OK, struct.pack("!I", len(sid)) + sid)
