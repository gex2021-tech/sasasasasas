from __future__ import annotations

import logging
import threading
import time
import uuid
from dataclasses import dataclass, field
from typing import Dict, Optional

from .event_log import EventRecord, SessionEventLog
from .heartbeat_scheduler import HeartbeatRelay, HeartbeatScheduler
from .jwt_util import account_from_jwt, shard_from_jwt
from .protocol import SessionAuthData
from .riot_proxy import RiotProxy
from .wine_manager import WineManager

log = logging.getLogger("session_manager")


@dataclass
class Session:
    session_id: str
    gateway_machine_id: bytes
    hwid_fingerprint: bytes = b""
    container_id: str = ""
    riot_token: str = ""
    client_puuid: str = ""
    client_jwt_at: float = 0.0
    pipe_auth_at: float = 0.0
    valorant_pid: int = 0
    client_ts_ms: int = 0
    region: str = ""
    riot_account: str = ""
    hostname: str = ""
    client_ip: str = ""
    jwt_push_count: int = 0
    pipe_auth_count: int = 0
    ping_count: int = 0
    ioctl_count: int = 0
    created_at: float = field(default_factory=time.time)
    last_activity: float = field(default_factory=time.time)
    scheduler: Optional[HeartbeatScheduler] = None


class SessionManager:
    def __init__(
        self,
        wine: WineManager,
        riot: RiotProxy,
        schedulers: Dict[str, HeartbeatScheduler],
        event_log: SessionEventLog,
        idle_timeout_sec: int,
    ):
        self.wine = wine
        self.riot = riot
        self._schedulers = schedulers
        self._event_log = event_log
        self.idle_timeout_sec = idle_timeout_sec
        self._sessions: Dict[str, Session] = {}
        self._lock = threading.Lock()
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._running = False
        self._status_tick = 0

    def _log_event(
        self,
        session_id: str,
        event_type: str,
        status: str,
        reason: str | None = None,
        meta: dict | None = None,
    ) -> None:
        self._event_log.log(
            session_id,
            EventRecord(
                type=event_type,
                sequence=0,
                timestamp_server=int(time.time() * 1000),
                status=status,
                reason=reason,
                meta=meta,
            ),
        )

    @staticmethod
    def _hwid_hex(blob: bytes) -> str:
        return blob.hex() if blob else ""

    def create_on_session_auth(self, auth: SessionAuthData, client_ip: str) -> Optional[str]:
        """SESSION_AUTH — cria sessão VM + container num único handshake."""
        region = auth.region or shard_from_jwt(auth.jwt)
        riot_account = auth.riot_account or account_from_jwt(auth.jwt)
        puuid = auth.puuid or ""

        session_id = str(uuid.uuid4())
        session = Session(
            session_id=session_id,
            gateway_machine_id=auth.gateway_machine_id,
            hwid_fingerprint=auth.hwid_fingerprint,
            riot_token=auth.jwt,
            client_puuid=puuid,
            client_jwt_at=time.time() if auth.jwt else 0.0,
            valorant_pid=auth.valorant_pid,
            pipe_auth_at=time.time(),
            pipe_auth_count=1,
            jwt_push_count=1 if auth.jwt else 0,
            client_ts_ms=auth.client_ts_ms,
            region=region,
            riot_account=riot_account,
            hostname=auth.hostname,
            client_ip=client_ip,
        )
        with self._lock:
            self._sessions[session_id] = session

        meta = {
            "client_ip": client_ip,
            "client_ts_ms": auth.client_ts_ms,
            "region": region,
            "puuid": puuid,
            "valorant_pid": auth.valorant_pid,
            "jwt_len": len(auth.jwt),
            "riot_account": riot_account,
            "hostname": auth.hostname,
            "gateway_machine_id": auth.gateway_machine_id.decode("utf-8", errors="replace"),
            "hwid_fingerprint_hex": self._hwid_hex(auth.hwid_fingerprint),
        }
        self._log_event(session_id, "session_auth", "created", None, meta)
        log.info(
            "session %s CREATED ip=%s region=%s account=%s pid=%d puuid=%s hwid=%s",
            session_id[:8],
            client_ip,
            region,
            riot_account[:24] if riot_account else "",
            auth.valorant_pid,
            puuid[:8] if puuid else "",
            self._hwid_hex(auth.hwid_fingerprint)[:16],
        )
        if not self._provision_container(session_id):
            self.destroy_session(session_id)
            return None
        return session_id

    def start(self) -> None:
        self._running = True
        self._thread.start()

    def stop(self) -> None:
        self._running = False

    def is_active(self, session_id: str) -> bool:
        with self._lock:
            return session_id in self._sessions

    def get(self, session_id: str) -> Optional[Session]:
        with self._lock:
            return self._sessions.get(session_id)

    def touch(self, session_id: str) -> None:
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.last_activity = time.time()

    def note_ping(self, session_id: str) -> None:
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.ping_count += 1
                s.last_activity = time.time()

    def note_ioctl(self, session_id: str, ioctl_code: int, in_len: int, out_len: int) -> None:
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.ioctl_count += 1
                s.last_activity = time.time()
        log.info(
            "session %s IOCTL 0x%X in=%d out=%d",
            session_id[:8],
            ioctl_code,
            in_len,
            out_len,
        )

    def update_jwt(self, session_id: str, jwt: str, puuid: str) -> bool:
        """JWT refresh após sessão já autenticada (container ativo)."""
        with self._lock:
            s = self._sessions.get(session_id)
            if not s:
                return False
            s.riot_token = jwt
            s.client_puuid = puuid
            s.client_jwt_at = time.time()
            s.jwt_push_count += 1
            s.last_activity = time.time()
            has_container = bool(s.container_id)
            container_id = s.container_id

        reason = f"puuid={puuid[:8] if puuid else ''} len={len(jwt)}"
        if has_container:
            self.riot.on_client_jwt(session_id, container_id, jwt, puuid)
            self._log_event(session_id, "jwt_update", "ok_container", reason)
            log.info(
                "session %s jwt updated (container live) puuid=%s len=%d",
                session_id[:8],
                puuid[:8] if puuid else "",
                len(jwt),
            )
        else:
            self._log_event(session_id, "jwt_update", "cached_active", reason)
            log.info(
                "session %s jwt cached (await container) puuid=%s len=%d",
                session_id[:8],
                puuid[:8] if puuid else "",
                len(jwt),
            )
        return True

    def note_pipe_auth_repeat(self, session_id: str, valorant_pid: int) -> bool:
        with self._lock:
            s = self._sessions.get(session_id)
            if not s:
                return False
            s.valorant_pid = valorant_pid
            s.pipe_auth_at = time.time()
            s.pipe_auth_count += 1
            s.last_activity = time.time()
            auth_n = s.pipe_auth_count
            cid = s.container_id

        self._log_event(session_id, "pipe_auth", "repeat", f"pid={valorant_pid} count={auth_n}")
        log.info(
            "session %s pipe auth repeat (container %s pid=%d)",
            session_id[:8],
            cid[:8],
            valorant_pid,
        )
        return True

    def _provision_container(self, session_id: str) -> bool:
        with self._lock:
            snap = self._sessions.get(session_id)
        if not snap:
            return False

        container_id = self.wine.create_container()
        profile = {
            "session_id": session_id,
            "gateway_machine_id": snap.gateway_machine_id,
            "hwid_fingerprint_hex": self._hwid_hex(snap.hwid_fingerprint),
            "client_puuid": snap.client_puuid,
            "valorant_pid": snap.valorant_pid,
            "region": snap.region,
            "riot_account": snap.riot_account,
            "hostname": snap.hostname,
            "client_ip": snap.client_ip,
            "client_ts_ms": snap.client_ts_ms,
            "jwt": snap.riot_token,
        }
        self.wine.bind_session_profile(container_id, profile)

        if not self.wine.wait_ready(container_id):
            self._log_event(session_id, "container", "not_ready", None)
            log.error("session %s container not ready", session_id[:8])
            self.wine.destroy_container(container_id)
            return False

        token = self.riot.authenticate(container_id, profile)
        scheduler = HeartbeatScheduler(session_id, container_id, self.riot)

        with self._lock:
            s = self._sessions.get(session_id)
            if not s:
                self.wine.destroy_container(container_id)
                return False
            s.container_id = container_id
            if token:
                s.riot_token = token
            s.scheduler = scheduler
            self._schedulers[session_id] = scheduler
            jwt = s.riot_token
            puuid = s.client_puuid
            pid = s.valorant_pid

        if jwt:
            self.riot.on_client_jwt(session_id, container_id, jwt, puuid)

        self._log_event(
            session_id,
            "container",
            "provisioned",
            f"cid={container_id[:8]} pid={pid} jwt_len={len(jwt)}",
        )
        log.info(
            "session %s container %s provisioned (pid=%d jwt_len=%d puuid=%s)",
            session_id[:8],
            container_id[:8],
            pid,
            len(jwt),
            puuid[:8] if puuid else "",
        )
        return True

    def destroy_session(self, session_id: str) -> None:
        with self._lock:
            s = self._sessions.pop(session_id, None)
            self._schedulers.pop(session_id, None)
        if s and s.container_id:
            self.wine.destroy_container(s.container_id)
            self._log_event(session_id, "session", "destroyed", f"cid={s.container_id[:8]}")
            log.info("session %s destroyed", session_id[:8])
        elif s:
            self._log_event(session_id, "session", "destroyed", "no_container")
            log.info("session %s destroyed (no container)", session_id[:8])

    def _loop(self) -> None:
        while self._running:
            with self._lock:
                ids = list(self._schedulers.keys())
            for sid in ids:
                sch = self._schedulers.get(sid)
                if sch:
                    sch.tick()
            self._expire_idle()
            self._status_tick += 1
            if self._status_tick % 120 == 0:
                self._log_status_snapshot()
            time.sleep(1.0)

    def _log_status_snapshot(self) -> None:
        with self._lock:
            sessions = len(self._sessions)
            containers = sum(1 for s in self._sessions.values() if s.container_id)
        if sessions == 0:
            return
        log.info(
            "status: active_sessions=%d containers=%d",
            sessions,
            containers,
        )

    def _expire_idle(self) -> None:
        now = time.time()
        expired_sessions: list[str] = []
        with self._lock:
            for sid, s in self._sessions.items():
                if now - s.last_activity > self.idle_timeout_sec:
                    expired_sessions.append(sid)
        for sid in expired_sessions:
            log.warning("session %s idle timeout", sid[:8])
            self.destroy_session(sid)
