from __future__ import annotations

import hashlib
import json
import logging
import threading
import time
import uuid
from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
from pathlib import Path
from typing import Any, Dict, List, Optional

from .event_log import EventRecord, SessionEventLog
from .gateway_envelope import build_gateway_envelope, post_gateway_auth
from .heartbeat_scheduler import HeartbeatRelay, HeartbeatScheduler
from .jwt_util import account_from_jwt, shard_from_jwt
from .machine_pool import select_machine_for_session
from .protocol import SessionAuthData
from .riot_proxy import RiotProxy
from .wine_manager import WineManager

log = logging.getLogger("session_manager")


class SessionState(Enum):
    """Session lifecycle states"""
    INITIALIZING = "initializing"
    AUTH_PENDING = "auth_pending"
    AUTHENTICATED = "authenticated"
    IN_QUEUE = "in_queue"
    IN_MATCH = "in_match"
    HEARTBEATING = "heartbeating"
    ZOMBIE = "zombie"  # Lost connection but not destroyed
    DESTROYED = "destroyed"


@dataclass
class TokenLifecycle:
    """Track token expiration and refresh"""
    f1_token: str = ""
    f1_issued_at: float = 0.0
    f1_expires_at: float = 0.0
    f1_ttl_seconds: int = 300  # 5 minutes

    f15_token: str = ""
    f15_issued_at: float = 0.0

    gateway_envelope: Optional[bytes] = None
    envelope_issued_at: float = 0.0
    envelope_ttl_seconds: int = 240  # 4 minutes

    def is_f1_expired(self) -> bool:
        """Check if F1 token is stale"""
        return time.time() > self.f1_expires_at if self.f1_expires_at > 0 else False

    def is_envelope_expired(self) -> bool:
        """Check if Gateway envelope is stale"""
        return time.time() > (self.envelope_issued_at + self.envelope_ttl_seconds) if self.envelope_issued_at > 0 else False

    def time_to_refresh_f1(self) -> float:
        """Seconds until F1 needs refresh"""
        return max(0.0, self.f1_expires_at - time.time()) if self.f1_expires_at > 0 else 0.0

    def time_to_refresh_envelope(self) -> float:
        """Seconds until envelope needs refresh"""
        return max(0.0, (self.envelope_issued_at + self.envelope_ttl_seconds) - time.time()) if self.envelope_issued_at > 0 else 0.0


@dataclass
class HeartbeatMetrics:
    """Track heartbeat health and detect gaps that trigger VAL 5"""
    last_hb_sent: float = 0.0
    last_hb_received: float = 0.0
    last_hb_timestamp: float = 0.0
    hb_count: int = 0
    hb_intervals: List[float] = field(default_factory=list)
    hb_gap_warning_threshold: float = 12.0  # 12 seconds
    hb_gap_critical_threshold: float = 15.0  # 15 seconds

    def record_heartbeat(self) -> Any:
        """Record heartbeat timing"""
        now = time.time()

        status: Any = True
        if self.last_hb_sent > 0:
            interval = now - self.last_hb_sent
            self.hb_intervals.append(interval)

            if len(self.hb_intervals) > 60:
                self.hb_intervals.pop(0)

            if interval > self.hb_gap_critical_threshold:
                status = False  # CRITICAL - will trigger VAL 5
            elif interval > self.hb_gap_warning_threshold:
                status = "warning"  # WARNING - at risk

        self.last_hb_sent = now
        self.hb_count += 1
        return status

    def avg_interval(self) -> float:
        """Average heartbeat interval"""
        if not self.hb_intervals:
            return 0.0
        return sum(self.hb_intervals) / len(self.hb_intervals)

    def get_health_status(self) -> Dict[str, Any]:
        """Current heartbeat health metrics"""
        return {
            "count": self.hb_count,
            "avg_interval": self.avg_interval(),
            "last_sent": self.last_hb_sent,
            "intervals_tracked": len(self.hb_intervals),
        }


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
    cpu_brand: str = ""
    cpu_model: str = ""
    gpu_brand: str = ""
    gpu_model: str = ""
    cpu_logical_count: int = 0
    jwt_push_count: int = 0
    pipe_auth_count: int = 0
    ping_count: int = 0
    ioctl_count: int = 0
    created_at: float = field(default_factory=time.time)
    last_activity: float = field(default_factory=time.time)
    scheduler: Optional[HeartbeatScheduler] = None
    # Gateway validation fields
    entitlements_token: str = ""
    id_token: str = ""
    gateway_response: bytes = b""
    gateway_auth_time: float = 0.0
    gateway_auth_ok: bool = False
    # Machine pool profile fields
    machine_idx: int = 0
    machine_profile: dict = field(default_factory=dict)
    # Dynamic build info
    build_info: dict = field(default_factory=dict)
    # Lifecycle & Heartbeat Metrics
    state: SessionState = SessionState.INITIALIZING
    token_lifecycle: TokenLifecycle = field(default_factory=TokenLifecycle)
    hb_metrics: HeartbeatMetrics = field(default_factory=HeartbeatMetrics)


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

    def start(self) -> None:
        if not self._running:
            self._running = True
            self._thread.start()
            log.info("SessionManager thread started")

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
        puuid = auth.puuid or ""
        if auth.jwt == "probe_jwt_placeholder" or puuid.startswith("00000000"):
            log.warning("Ignoring dummy probe session_auth from %s", client_ip)
            return None

        region = auth.region or shard_from_jwt(auth.jwt)
        riot_account = auth.riot_account or account_from_jwt(auth.jwt)

        # Purge stale/previous active sessions for the same PUUID to avoid duplicate sessions (VAL 5 trigger)
        if puuid:
            with self._lock:
                stale_ids = [sid for sid, s in self._sessions.items() if s.client_puuid == puuid]
            for sid in stale_ids:
                log.info("purging old duplicate session=%s for puuid=%s", sid[:8], puuid[:8])
                self.destroy_session(sid)

        session_id = uuid.uuid4().hex[:8]
        machine_idx, machine_profile = select_machine_for_session(puuid, session_id)

        entitlements_token = getattr(auth, 'entitlements_token', '') or auth.jwt
        id_token = getattr(auth, 'id_token', '') or ""

        build_info = {
            "branch": getattr(auth, 'build_branch', '') or "release",
            "changelist": getattr(auth, 'build_changelist', 0) or 0,
            "major": getattr(auth, 'build_major', 0) or 1,
            "minor": getattr(auth, 'build_minor', 0) or 18,
            "patch": getattr(auth, 'build_patch', 0) or 5,
            "flags": getattr(auth, 'build_flags', 0) or 0,
        }

        token_lifecycle = TokenLifecycle(
            envelope_issued_at=time.time(),
            envelope_ttl_seconds=240,
        )

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
            hostname=auth.hostname if auth.hostname else machine_profile.get("hostname", "WIN-PC"),
            client_ip=client_ip,
            cpu_brand=auth.cpu_brand if auth.cpu_brand else machine_profile.get("cpu_brand", "Intel"),
            cpu_model=auth.cpu_model if auth.cpu_model else machine_profile.get("cpu_model", "Intel Core"),
            gpu_brand=auth.gpu_brand if auth.gpu_brand else machine_profile.get("gpu_brand", "Intel"),
            gpu_model=auth.gpu_model if auth.gpu_model else machine_profile.get("gpu_model", "Intel UHD"),
            cpu_logical_count=auth.cpu_logical_count if auth.cpu_logical_count else machine_profile.get("cpu_logical_count", 12),
            entitlements_token=entitlements_token,
            id_token=id_token,
            gateway_auth_ok=False,
            gateway_auth_time=0.0,
            machine_idx=machine_idx,
            machine_profile=machine_profile,
            build_info=build_info,
            state=SessionState.INITIALIZING,
            token_lifecycle=token_lifecycle,
        )
        with self._lock:
            self._sessions[session_id] = session

        meta = {
            "account": riot_account,
            "region": region,
            "hwid": self._hwid_hex(auth.hwid_fingerprint)[:16],
            "ip": client_ip,
            "pid": auth.valorant_pid,
            "machine_idx": machine_idx,
        }
        self._log_event(session_id, "session", "created", None, meta)
        log.info(
            "session %s CREATED ip=%s region=%s account=%s pid=%d puuid=%s hwid=%s machine_idx=%d",
            session_id[:8],
            client_ip,
            region,
            riot_account[:8],
            auth.valorant_pid,
            puuid[:8] if puuid else "",
            self._hwid_hex(auth.hwid_fingerprint)[:16],
            machine_idx,
        )

        log.info(
            "session %s HWINFO cpu='%s %s' (%d threads) gpu='%s %s'",
            session_id[:8],
            session.cpu_brand,
            session.cpu_model,
            session.cpu_logical_count,
            session.gpu_brand,
            session.gpu_model,
        )

        if not self._provision_container(session_id, machine_profile):
            self.destroy_session(session_id)
            return None

        session.state = SessionState.AUTHENTICATED
        return session_id

    def is_active(self, session_id: str) -> bool:
        with self._lock:
            s = self._sessions.get(session_id)
            return s is not None and s.state != SessionState.DESTROYED

    def touch(self, session_id: str) -> None:
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.last_activity = time.time()

    def note_ioctl(self, session_id: str, code: int, in_len: int, out_len: int) -> None:
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.ioctl_count += 1
                s.last_activity = time.time()
                s.hb_metrics.record_heartbeat()
                if code == 0x222000:
                    s.state = SessionState.HEARTBEATING
        log.info("session %s IOCTL 0x%X in=%d out=%d", session_id[:8], code, in_len, out_len)

    def note_ping(self, session_id: str) -> None:
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.ping_count += 1
                s.last_activity = time.time()

    def note_pipe_auth_repeat(self, session_id: str, pid: int) -> bool:
        with self._lock:
            s = self._sessions.get(session_id)
            if not s:
                return False
            s.pipe_auth_count += 1
            s.last_activity = time.time()
            if pid and not s.valorant_pid:
                s.valorant_pid = pid
        self._log_event(session_id, "pipe_auth", "ok", f"pid={pid}")
        return True

    def update_jwt(self, session_id: str, jwt: str, puuid: str = "") -> bool:
        with self._lock:
            s = self._sessions.get(session_id)
            if not s:
                return False
            s.riot_token = jwt
            s.client_jwt_at = time.time()
            s.jwt_push_count += 1
            s.last_activity = time.time()
            s.token_lifecycle.envelope_issued_at = time.time()
            cid = s.container_id

        if cid and jwt:
            self.wine.push_jwt_to_container(cid, jwt, puuid or s.client_puuid)
            self.riot.on_client_jwt(session_id, cid, jwt, puuid or s.client_puuid)
            self._log_event(session_id, "jwt", "updated", f"len={len(jwt)}")
            log.info("session %s updated JWT len=%d (push_count=%d)", session_id[:8], len(jwt), s.jwt_push_count)

        # Update crypto session
        from .vgc_crypto import _crypto
        _crypto.update_jwt(session_id, jwt, puuid or s.client_puuid)

        return True

    def _provision_container(self, session_id: str, machine_profile: dict = None) -> bool:
        with self._lock:
            snap = self._sessions.get(session_id)
        if not snap:
            return False

        start_time = time.time()

        profile = {
            "session_id": session_id,
            "gateway_machine_id": snap.gateway_machine_id,
            "hwid_fingerprint": snap.hwid_fingerprint,
            "riot_token": snap.riot_token,
            "client_puuid": snap.client_puuid,
            "region": snap.region,
            "riot_account": snap.riot_account,
            "hostname": snap.hostname,
            "client_ip": snap.client_ip,
            "cpu_brand": snap.cpu_brand,
            "cpu_model": snap.cpu_model,
            "gpu_brand": snap.gpu_brand,
            "gpu_model": snap.gpu_model,
            "cpu_logical_count": snap.cpu_logical_count,
        }
        if machine_profile:
            profile.update(machine_profile)

        container_id = self.wine.create_container_with_profile(profile)

        from .vgc_crypto import _crypto
        _crypto.mount(
            session_id=session_id,
            puuid=snap.client_puuid,
            jwt=snap.riot_token,
            hwid_blob=snap.hwid_fingerprint
        )

        entitlements_token = snap.entitlements_token or snap.riot_token
        id_token = snap.id_token or ""

        gateway_envelope = build_gateway_envelope(
            session_id=session_id,
            hwid_hex=self._hwid_hex(snap.hwid_fingerprint),
            puuid=snap.client_puuid,
            region=snap.region,
            build_info=snap.build_info if snap.build_info else {"major": 1, "minor": 18, "patch": 5},
            rsa_spki_pem=snap.gateway_machine_id if snap.gateway_machine_id else b"",
            timestamp_ms=int(time.time() * 1000),
            entitlements_token=entitlements_token,
            id_token=id_token,
        )

        log.info(
            "session %s building gateway envelope (tokens from vClient) envelope_size=%d",
            session_id[:8],
            len(gateway_envelope)
        )

        status_code, response_body = post_gateway_auth(
            region=snap.region,
            gateway_envelope=gateway_envelope,
            entitlements_token=entitlements_token,
            id_token=id_token,
            puuid=snap.client_puuid,
        )

        if status_code != 200 or not response_body or len(response_body) == 0:
            log.error(
                "session %s GATEWAY AUTH FAILED HTTP %d (body_len=%d) — destroying container",
                session_id[:8],
                status_code,
                len(response_body) if response_body else 0
            )
            self.wine.destroy_container(container_id)
            return False

        if len(response_body) < 100:
            log.warning(
                "session %s Gateway response small (%d bytes) — monitor for VAL 5",
                session_id[:8],
                len(response_body)
            )

        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.gateway_response = response_body
                s.gateway_auth_time = time.time()
                s.gateway_auth_ok = True
                s.token_lifecycle.gateway_envelope = response_body

        _crypto.set_gateway_token(session_id, response_body)

        token = self.wine.wait_for_token(container_id, timeout_sec=2.0)
        scheduler = HeartbeatScheduler(
            session_id=session_id,
            container_id=container_id,
            wine=self.wine,
            riot=self.riot,
            event_log=self._event_log,
        )

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

        # Send FIRST heartbeat immediately to prevent VAL 5
        log.info("session %s sending IMMEDIATE first heartbeat (force=True) to prevent VAL 5", session_id[:8])
        try:
            scheduler.send_heartbeat(force=True)
            log.info("session %s first heartbeat sent successfully", session_id[:8])
        except Exception as e:
            log.error("session %s first heartbeat FAILED: %s — continuing anyway", session_id[:8], e)

        provision_time = time.time() - start_time
        self._log_event(
            session_id,
            "container",
            "provisioned",
            f"cid={container_id[:8]} pid={pid} jwt_len={len(jwt)} provision_ms={int(provision_time*1000)}",
        )
        log.info(
            "session %s container %s provisioned in %.2fs (pid=%d jwt_len=%d puuid=%s machine_idx=%d)",
            session_id[:8],
            container_id[:8],
            provision_time,
            pid,
            len(jwt),
            puuid[:8] if puuid else "",
            snap.machine_idx,
        )

        if provision_time > 15.0:
            log.warning(
                "session %s SLOW PROVISIONING (%.2fs) — exceeded safety threshold. May trigger VAL 5!",
                session_id[:8],
                provision_time
            )
        elif provision_time > 10.0:
            log.warning(
                "session %s slow provisioning (%.2fs) — monitor for VAL 5",
                session_id[:8],
                provision_time
            )

        return True

    def destroy_session(self, session_id: str) -> None:
        with self._lock:
            s = self._sessions.pop(session_id, None)
            self._schedulers.pop(session_id, None)

        if s:
            s.state = SessionState.DESTROYED

        if s and s.container_id:
            self.wine.destroy_container(s.container_id)
            self._log_event(session_id, "session", "destroyed", f"cid={s.container_id[:8]}")
            log.info("session %s destroyed", session_id[:8])
        elif s:
            self._log_event(session_id, "session", "destroyed", "no_container")
            log.info("session %s destroyed (no container)", session_id[:8])

        # Clean up VGCDriver persistent state
        from .vgc_driver import _driver
        _driver.cleanup_session(session_id)

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

            now = time.time()
            at_risk = []
            for sid, s in self._sessions.items():
                time_since_auth = now - s.client_jwt_at if s.client_jwt_at else 999
                time_since_activity = now - s.last_activity

                if time_since_auth > 240 or time_since_activity > 180:
                    at_risk.append((sid, time_since_auth, time_since_activity))

        if sessions == 0:
            return

        log.info(
            "status: active_sessions=%d containers=%d",
            sessions,
            containers,
        )

        for sid, jwt_age, activity_age in at_risk:
            log.warning(
                "session %s VAL 5 RISK: jwt_age=%.1fs activity_age=%.1fs",
                sid[:8],
                jwt_age,
                activity_age
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
