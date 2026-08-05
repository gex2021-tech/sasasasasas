"""Shared socket helpers for the tunnel/IPC protocols."""
from __future__ import annotations

import socket
import ssl
from typing import Optional, Tuple

from .protocol import HEADER


def create_insecure_tls_context() -> ssl.SSLContext:
    """TLS context for the tunnel: encryption only, self-signed certs accepted."""
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    return ctx


def connect_tls(host: str, port: int, timeout: Optional[float] = None) -> ssl.SSLSocket:
    """Open a TLS connection to the tunnel server."""
    raw = socket.create_connection((host, port), timeout=timeout)
    return create_insecure_tls_context().wrap_socket(raw, server_hostname=host)


def recv_exact(sock: socket.socket, size: int, error_msg: str = "eof") -> bytes:
    """Read exactly `size` bytes, raising ConnectionError on premature EOF."""
    buf = bytearray()
    while len(buf) < size:
        chunk = sock.recv(size - len(buf))
        if not chunk:
            raise ConnectionError(error_msg)
        buf.extend(chunk)
    return bytes(buf)


def recv_message(sock: socket.socket) -> Optional[Tuple[int, bytes]]:
    """Read one framed protocol message. Returns (msg_type, payload) or None on EOF."""
    try:
        header = recv_exact(sock, HEADER.size)
    except ConnectionError:
        return None
    msg_type, payload_len = HEADER.unpack(header)
    payload = recv_exact(sock, payload_len) if payload_len else b""
    return msg_type, payload
