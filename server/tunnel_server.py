from __future__ import annotations

import logging
import socket
import ssl
import struct
import threading
from pathlib import Path
from typing import Optional

from .heartbeat_scheduler import HeartbeatRelay
from .protocol import (
    HEADER,
    MsgType,
    pack,
    pack_heartbeat_buffer,
    pack_ioctl_resp,
    pack_jwt_ok,
    pack_pipe_auth_ok,
    pack_pong,
    pack_session_auth_ok,
    parse_ioctl,
    parse_jwt_update,
    parse_pipe_auth,
    parse_session_auth,
    parse_sync,
    unpack_header,
)
from .session_manager import SessionManager

log = logging.getLogger("tunnel")


class TunnelServer:
    def __init__(
        self,
        host: str,
        port: int,
        auth_key: str,
        tls_cert: Path,
        tls_key: Path,
        session_mgr: SessionManager,
        relay: HeartbeatRelay,
        max_clients: int,
    ):
        self.host = host
        self.port = port
        self.auth_key = auth_key
        self.tls_cert = tls_cert
        self.tls_key = tls_key
        self.session_mgr = session_mgr
        self.relay = relay
        self.max_clients = max_clients
        self._clients = 0
        self._lock = threading.Lock()

    def serve_forever(self) -> None:
        ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ctx.load_cert_chain(certfile=str(self.tls_cert), keyfile=str(self.tls_key))
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind((self.host, self.port))
            sock.listen(64)
            log.info("tunnel TLS listening %s:%d", self.host, self.port)
            while True:
                raw, addr = sock.accept()
                with self._lock:
                    if self._clients >= self.max_clients:
                        raw.close()
                        continue
                    self._clients += 1
                threading.Thread(target=self._client, args=(raw, addr, ctx), daemon=True).start()

    def _client(self, raw: socket.socket, addr, ctx: ssl.SSLContext) -> None:
        session_id: Optional[str] = None
        try:
            with ctx.wrap_socket(raw, server_side=True) as conn:
                conn.settimeout(120.0)
                log.info("client connect %s", addr)
                while True:
                    hdr = self._read(conn, HEADER.size)
                    mt, plen = unpack_header(hdr)
                    payload = self._read(conn, plen) if plen else b""

                    if mt == MsgType.SESSION_AUTH:
                        auth = parse_session_auth(payload)
                        if auth.auth_key != self.auth_key:
                            log.warning("SESSION_AUTH auth_failed from %s", addr)
                            conn.sendall(pack(MsgType.ERROR, b"auth_failed"))
                            break
                        if not auth.jwt:
                            conn.sendall(pack(MsgType.ERROR, b"jwt_empty"))
                            continue
                        client_ip = addr[0] if isinstance(addr, tuple) else str(addr)
                        log.info(
                            "SESSION_AUTH from %s pid=%d puuid=%s jwt_len=%d",
                            client_ip,
                            auth.valorant_pid,
                            auth.puuid[:8] if auth.puuid else "",
                            len(auth.jwt),
                        )
                        sid = self.session_mgr.create_on_session_auth(auth, client_ip)
                        if sid:
                            session_id = sid
                            conn.sendall(pack_session_auth_ok(sid))
                        else:
                            conn.sendall(pack(MsgType.ERROR, b"session_auth_failed"))
                    elif mt == MsgType.HELLO:
                        log.warning("HELLO rejected from %s (use SESSION_AUTH)", addr)
                        conn.sendall(pack(MsgType.ERROR, b"use_session_auth"))
                    elif mt == MsgType.SYNC:
                        if not session_id:
                            conn.sendall(pack(MsgType.ERROR, b"not_authenticated"))
                            continue
                        sid, last_seq = parse_sync(payload)
                        session_id = sid
                        if not self.session_mgr.is_active(sid):
                            log.info("SYNC ignored session=%s (not active)", sid[:8])
                            continue
                        self.session_mgr.touch(sid)
                        buffered = self.relay.on_reconnect(sid, last_seq)
                        log.info("SYNC session=%s last_seq=%d buffered=%d", sid[:8], last_seq, len(buffered))
                        for seq, data in buffered:
                            conn.sendall(pack_heartbeat_buffer(seq, data))
                    elif mt == MsgType.IOCTL:
                        if not session_id or not self.session_mgr.is_active(session_id):
                            conn.sendall(pack(MsgType.ERROR, b"not_authenticated"))
                            continue
                        ioctl_code, data = parse_ioctl(payload)
                        self.session_mgr.touch(session_id)
                        resp = self.relay.on_ioctl(session_id, ioctl_code, data)
                        self.session_mgr.note_ioctl(session_id, ioctl_code, len(data), len(resp))
                        conn.sendall(pack_ioctl_resp(resp))
                    elif mt == MsgType.PING:
                        if session_id and self.session_mgr.is_active(session_id):
                            self.session_mgr.note_ping(session_id)
                        conn.sendall(pack_pong())
                    elif mt == MsgType.JWT_UPDATE:
                        if not session_id or not self.session_mgr.is_active(session_id):
                            conn.sendall(pack(MsgType.ERROR, b"not_authenticated"))
                            continue
                        jwt, puuid = parse_jwt_update(payload)
                        if not jwt:
                            conn.sendall(pack(MsgType.ERROR, b"jwt_empty"))
                            continue
                        if self.session_mgr.update_jwt(session_id, jwt, puuid):
                            conn.sendall(pack_jwt_ok())
                        else:
                            conn.sendall(pack(MsgType.ERROR, b"session_missing"))
                    elif mt == MsgType.PIPE_AUTH:
                        if not session_id or not self.session_mgr.is_active(session_id):
                            conn.sendall(pack(MsgType.ERROR, b"not_authenticated"))
                            continue
                        valorant_pid = parse_pipe_auth(payload)
                        log.info("PIPE_AUTH session=%s valorant_pid=%d", session_id[:8], valorant_pid)
                        if self.session_mgr.note_pipe_auth_repeat(session_id, valorant_pid):
                            conn.sendall(pack_pipe_auth_ok())
                        else:
                            conn.sendall(pack(MsgType.ERROR, b"pipe_auth_failed"))
                    else:
                        log.warning("unknown msg type=%d plen=%d", mt, plen)
        except (ConnectionError, ssl.SSLError, OSError, struct.error) as e:
            log.info("disconnect %s: %s", addr, e)
        finally:
            if session_id:
                log.info("tunnel closed session=%s stays on server", session_id[:8])
            with self._lock:
                self._clients -= 1

    def _read(self, conn: ssl.SSLSocket, n: int) -> bytes:
        buf = b""
        while len(buf) < n:
            chunk = conn.recv(n - len(buf))
            if not chunk:
                raise ConnectionError("eof")
            buf += chunk
        return buf
