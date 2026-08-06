from __future__ import annotations

import logging
import socket
import ssl
import struct
import threading
from pathlib import Path
import time
from typing import Optional

from .gateway_envelope import build_gateway_envelope
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
                    try:
                        mt, plen = unpack_header(hdr)
                    except struct.error as e:
                        log.error("header unpack failed from %s: %s", addr, e)
                        conn.sendall(pack(MsgType.ERROR, b"invalid_header"))
                        break

                    # VALIDATION: Message type bounds check
                    valid_types = {
                        MsgType.SESSION_AUTH,
                        MsgType.HELLO,
                        MsgType.SYNC,
                        MsgType.IOCTL,
                        MsgType.PING,
                        MsgType.JWT_UPDATE,
                        MsgType.PIPE_AUTH,
                    }

                    if mt not in valid_types:
                        log.warning("invalid message type=%d plen=%d from %s", mt, plen, addr)
                        conn.sendall(pack(MsgType.ERROR, b"invalid_msg_type"))
                        break

                    # VALIDATION: Payload length sanity check (prevent memory exhaustion)
                    if plen > 1_000_000:
                        log.error("payload too large plen=%d from %s — dropping connection", plen, addr)
                        conn.sendall(pack(MsgType.ERROR, b"payload_too_large"))
                        break

                    payload = self._read(conn, plen) if plen else b""

                    if mt == MsgType.SESSION_AUTH:
                        try:
                            auth = parse_session_auth(payload)
                        except Exception as e:
                            log.error("SESSION_AUTH parse failed from %s: %s", addr, e)
                            conn.sendall(pack(MsgType.ERROR, b"parse_error"))
                            break

                        if not auth.auth_key or not self.auth_key or auth.auth_key != self.auth_key:
                            log.warning("SESSION_AUTH auth_failed from %s (key mismatch/missing)", addr)
                            conn.sendall(pack(MsgType.ERROR, b"auth_failed"))
                            break

                        if not auth.jwt or len(auth.jwt) < 16:
                            log.warning("SESSION_AUTH invalid jwt from %s (len=%d)", addr, len(auth.jwt) if auth.jwt else 0)
                            conn.sendall(pack(MsgType.ERROR, b"jwt_invalid"))
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
                            timestamp_ms = int(time.time() * 1000)
                            build_info = {
                                "branch": auth.build_branch or "release",
                                "changelist": auth.build_changelist or 0,
                                "major": auth.build_major or 1,
                                "minor": auth.build_minor or 18,
                                "patch": auth.build_patch or 5,
                                "flags": auth.build_flags or 0,
                            }
                            ent_tok = getattr(auth, 'entitlements_token', '') or auth.jwt
                            id_tok = getattr(auth, 'id_token', '')
                            env = build_gateway_envelope(
                                session_id=sid,
                                hwid_hex=auth.hwid_fingerprint.hex() if auth.hwid_fingerprint else "",
                                puuid=auth.puuid,
                                region=auth.region.strip() if auth.region else "la",
                                build_info=build_info,
                                rsa_spki_pem=auth.rsa_spki_pem,
                                timestamp_ms=timestamp_ms,
                                entitlements_token=ent_tok,
                                id_token=id_tok,
                            )
                            conn.sendall(pack_session_auth_ok(sid, env))
                        else:
                            conn.sendall(pack(MsgType.ERROR, b"session_auth_failed"))

                    elif mt == MsgType.HELLO:
                        log.warning("HELLO rejected from %s (use SESSION_AUTH)", addr)
                        conn.sendall(pack(MsgType.ERROR, b"use_session_auth"))

                    elif mt == MsgType.SYNC:
                        if not session_id:
                            conn.sendall(pack(MsgType.ERROR, b"not_authenticated"))
                            continue

                        try:
                            sid, last_seq = parse_sync(payload)
                        except Exception as e:
                            log.error("SYNC parse failed: %s", e)
                            conn.sendall(pack(MsgType.ERROR, b"parse_error"))
                            continue

                        # CRITICAL: Validate SYNC session_id matches authenticated session
                        if sid != session_id:
                            log.warning("SYNC hijack attempt: auth_session=%s sync_session=%s from %s", session_id[:8], sid[:8], addr)
                            conn.sendall(pack(MsgType.ERROR, b"session_mismatch"))
                            continue

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

                        try:
                            ioctl_code, data = parse_ioctl(payload)
                        except Exception as e:
                            log.error("IOCTL parse failed session=%s: %s", session_id[:8], e)
                            conn.sendall(pack(MsgType.ERROR, b"parse_error"))
                            continue

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

                        try:
                            jwt, puuid = parse_jwt_update(payload)
                        except Exception as e:
                            log.error("JWT_UPDATE parse failed session=%s: %s", session_id[:8], e)
                            conn.sendall(pack(MsgType.ERROR, b"parse_error"))
                            continue

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

                        try:
                            valorant_pid = parse_pipe_auth(payload)
                        except Exception as e:
                            log.error("PIPE_AUTH parse failed session=%s: %s", session_id[:8], e)
                            conn.sendall(pack(MsgType.ERROR, b"parse_error"))
                            continue

                        log.info("PIPE_AUTH session=%s valorant_pid=%d", session_id[:8], valorant_pid)
                        if self.session_mgr.note_pipe_auth_repeat(session_id, valorant_pid):
                            conn.sendall(pack_pipe_auth_ok())
                        else:
                            conn.sendall(pack(MsgType.ERROR, b"pipe_auth_failed"))

                    else:
                        log.warning("unknown msg type=%d plen=%d", mt, plen)

        except (ConnectionError, ssl.SSLError, OSError, struct.error, socket.timeout) as e:
            if isinstance(e, socket.timeout):
                log.info("client timeout %s (idle 120s)", addr)
            else:
                log.info("disconnect %s: %s", addr, type(e).__name__)
        finally:
            if session_id:
                log.info("tunnel closed session=%s stays on server", session_id[:8])
            with self._lock:
                self._clients -= 1

    def _read(self, conn: ssl.SSLSocket, n: int) -> bytes:
        """Read exactly n bytes from socket with timeout enforcement."""
        if n < 0 or n > 1_000_000:
            raise ValueError(f"invalid read size {n}")

        buf = b""
        while len(buf) < n:
            try:
                chunk = conn.recv(n - len(buf))
                if not chunk:
                    raise ConnectionError(f"eof: expected {n} bytes, got {len(buf)}")
                buf += chunk
            except socket.timeout:
                raise ConnectionError(f"timeout: expected {n} bytes, got {len(buf)}")

        return buf
