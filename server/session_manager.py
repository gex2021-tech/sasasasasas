from __future__ import annotations

import hashlib
import json
import logging
import os
import threading
import time
import uuid
from dataclasses import dataclass, field
from enum import Enum
from typing import Dict, Optional

from .event_log import EventRecord, SessionEventLog
from .heartbeat_scheduler import HeartbeatRelay, HeartbeatScheduler
from .jwt_util import account_from_jwt, shard_from_jwt
from .protocol import SessionAuthData
from .riot_proxy import RiotProxy
from .wine_manager import WineManager
from .machine_pool import select_machine_for_session
from .gateway_envelope import build_gateway_envelope, post_gateway_auth

log = logging.getLogger("session_manager")


class SessionState(Enum):
    INITIALIZING = "initializing"
    AUTH_PENDING = "auth_pending"
    AUTHENTICATED = "authenticated"
    IN_QUEUE = "in_queue"
    IN_MATCH = "in_match"
    HEARTBEATING = "heartbeating"
    ZOMBIE = "zombie"
    DESTROYED = "destroyed"


@dataclass
class TokenLifecycle:
    f1_issued_at: float = field(default_factory=time.time)
    envelope_issued_at: float = field(default_factory=time.time)
    
    def is_f1_expired(self) -> bool:
        return (time.time() - self.f1_issued_at) >= 300
        
    def is_envelope_expired(self) -> bool:
        return (time.time() - self.envelope_issued_at) >= 240
        
    def time_to_refresh_f1(self) -> float:
        return max(0.0, 300 - (time.time() - self.f1_issued_at))
        
    def time_to_refresh_envelope(self) -> float:
        return max(0.0, 240 - (time.time() - self.envelope_issued_at))


@dataclass
class HeartbeatMetrics:
    last_hb_sent: float = 0.0
    last_hb_received: float = 0.0
    hb_count: int = 0
    hb_intervals: list[float] = field(default_factory=list)
    
    def record_heartbeat(self) -> bool | str:
        now = time.time()
        if self.last_hb_sent > 0:
            gap = now - self.last_hb_sent
            self.hb_intervals.append(gap)
            if len(self.hb_intervals) > 60:
                self.hb_intervals.pop(0)
                
            self.last_hb_sent = now
            if gap >= 15.0:
                return False
            elif gap >= 12.0:
                return "warning"
        else:
            self.last_hb_sent = now
        self.hb_count += 1
        return True
        
    def avg_interval(self) -> float:
        if not self.hb_intervals:
            return 0.0
        return sum(self.hb_intervals) / len(self.hb_intervals)
        
    def get_health_status(self) -> str:
        if not self.hb_intervals:
            return "unknown"
        avg = self.avg_interval()
        if avg >= 12.0:
            return "critical"
        elif avg >= 10.0:
            return "warning"
        return "healthy"


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
    
    # NEW ARCHITECTURE FIELDS
    state: SessionState = SessionState.INITIALIZING
    token_lifecycle: TokenLifecycle = field(default_factory=TokenLifecycle)
    hb_metrics: HeartbeatMetrics = field(default_factory=HeartbeatMetrics)
    _thread: Optional[threading.Thread] = None
    _stop_event: threading.Event = field(default_factory=threading.Event)
    
    # SYNC Recovery Buffer
    sync_buffer: list[dict] = field(default_factory=list)
    sync_lock: threading.Lock = field(default_factory=threading.Lock)

    def add_sync_payload(self, payload: dict) -> None:
        with self.sync_lock:
            self.sync_buffer.append(payload)
            if len(self.sync_buffer) > 20:
                self.sync_buffer.pop(0)


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
        self._purged_last_min = 0

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

    def _log_session_json(self, session: Session) -> None:
        try:
            os.makedirs("logs/sessions", exist_ok=True)
            log_path = f"logs/sessions/{session.session_id}.jsonl"
            with open(log_path, "a") as f:
                record = {
                    "ts": time.time(),
                    "state": session.state.value,
                    "avg_hb": session.hb_metrics.avg_interval(),
                    "hb_count": session.hb_metrics.hb_count,
                    "f1_expired": session.token_lifecycle.is_f1_expired(),
                    "envelope_expired": session.token_lifecycle.is_envelope_expired(),
                }
                f.write(json.dumps(record) + "\n")
        except Exception:
            pass

    def _session_loop(self, session_id: str):
        """Dedicated thread per session — handles heartbeat ticking and token lifecycle."""
        session = self.get(session_id)
        if not session:
            return
        
        while not session._stop_event.is_set():
            # 1. Tick heartbeat scheduler
            if session.scheduler:
                session.scheduler.tick()
            
            # 2. Record heartbeat metrics
            hb_result = session.hb_metrics.record_heartbeat()
            if hb_result is False:
                log.critical("session %s CRITICAL heartbeat gap detected — VAL 5 imminent!", session_id[:8])
            elif hb_result == "warning":
                log.warning("session %s heartbeat gap warning", session_id[:8])
            
            # 3. Check token lifecycle
            if session.token_lifecycle.is_f1_expired():
                log.warning("session %s F1 token expired — needs JWT refresh", session_id[:8])
            if session.token_lifecycle.is_envelope_expired():
                log.warning("session %s gateway envelope expired — needs re-auth", session_id[:8])
            
            # 4. Auto-transitions state
            if session.state == SessionState.AUTHENTICATED and session.scheduler:
                session.state = SessionState.HEARTBEATING
                
            # 5. Logs structured JSON events to per-session log files
            self._log_session_json(session)
            
            # Sleep 1 second
            session._stop_event.wait(1.0)

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
                to_destroy = [s_id for s_id, s in self._sessions.items() if s.client_puuid == puuid]
            for old_sid in to_destroy:
                log.info("purging old duplicate session=%s for puuid=%s", old_sid[:8], puuid[:8])
                self.destroy_session(old_sid)

        session_id = str(uuid.uuid4())
        
        # CRITICAL FIX: Select machine profile from pool of 500 (paid emulator logic)
        machine_idx, machine_profile = select_machine_for_session(session_seed=int(hashlib.md5(session_id.encode()).hexdigest()[:8], 16))
        log.info(f"[GW] generating 500 machine entries in memory...")
        log.info(f"[GW] selected machine idx={machine_idx} (500 entries)")
        
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
            
            # New Architecture: initialized components
            state=SessionState.AUTHENTICATED,
            token_lifecycle=TokenLifecycle(),
            hb_metrics=HeartbeatMetrics(),
        )
        
        with self._lock:
            self._sessions[session_id] = session

        # Start dedicated session thread
        session._thread = threading.Thread(target=self._session_loop, args=(session_id,), daemon=True)
        session._thread.start()

        meta = {
            "client_ip": client_ip,
            "client_ts_ms": auth.client_ts_ms,
            "region": region,
            "puuid": puuid,
            "valorant_pid": auth.valorant_pid,
            "jwt_len": len(auth.jwt),
            "riot_account": riot_account,
            "hostname": session.hostname,
            "gateway_machine_id": auth.gateway_machine_id.decode("utf-8", errors="replace"),
            "hwid_fingerprint_hex": self._hwid_hex(auth.hwid_fingerprint),
            "machine_pool_idx": machine_idx,
        }
        self._log_event(session_id, "session_auth", "created", None, meta)
        log.info(
            "session %s CREATED ip=%s region=%s account=%s pid=%d puuid=%s hwid=%s machine_idx=%d",
            session_id[:8],
            client_ip,
            region,
            riot_account[:24] if riot_account else "",
            auth.valorant_pid,
            puuid[:8] if puuid else "",
            self._hwid_hex(auth.hwid_fingerprint)[:16],
            machine_idx
        )
        if auth.cpu_brand or auth.gpu_brand:
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

        log.info(
            "session %s pipe auth repeat (container %s pid=%d)",
            session_id[:8],
            cid[:8],
            valorant_pid,
        )
        return True

    def _provision_container(self, session_id: str, machine_profile: dict = None) -> bool:
        """Provision Wine container + gateway auth using Session cached attributes."""
        with self._lock:
            snap = self._sessions.get(session_id)
        if not snap:
            return False

        # FAST PROVISIONING: Start immediately to avoid VAL 5 timeout
        start_time = time.time()
        
        container_id = self.wine.create_container()
        
        # Use machine profile from Session if omitted
        m_profile = machine_profile if machine_profile else snap.machine_profile

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
            # Merge machine profile fields (from pool of 500)
            "bios_info": m_profile.get("bios_info") if m_profile else None,
            "motherboard": m_profile.get("motherboard") if m_profile else None,
            "volume_serial": m_profile.get("volume_serial") if m_profile else None,
            "cpu_brand": snap.cpu_brand,
            "cpu_model": snap.cpu_model,
            "gpu_brand": snap.gpu_brand,
            "gpu_model": snap.gpu_model,
            "cpu_logical_count": snap.cpu_logical_count,
        }
        self.wine.bind_session_profile(container_id, profile)

        if not self.wine.wait_ready(container_id):
            self._log_event(session_id, "container", "not_ready", None)
            log.error("session %s container not ready", session_id[:8])
            self.wine.destroy_container(container_id)
            return False

        # CRITICAL: Build gateway envelope with REAL tokens from vClient (cached in Session)
        entitlements_token = snap.entitlements_token or snap.riot_token
        id_token = snap.id_token or ""

        # Build dynamic protobuf gateway envelope
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

        # POST envelope to real Riot gateway
        status_code, response_body = post_gateway_auth(
            gateway_envelope,
            region=snap.region,
            session_id=session_id,
            entitlements_token=entitlements_token,
            id_token=id_token,
            puuid=snap.client_puuid
        )

        # VALIDATION: Gateway response MUST be non-empty + status 200
        if status_code != 200 or not response_body:
            log.error(
                "session %s gateway auth FAILED status=%d response_len=%d - destroying container",
                session_id[:8],
                status_code,
                len(response_body) if response_body else 0
            )
            self._log_event(
                session_id,
                "gateway",
                "auth_failed",
                f"status={status_code} len={len(response_body) if response_body else 0}"
            )
            self.wine.destroy_container(container_id)
            return False

        if len(response_body) < 32:
            log.warning(
                "session %s gateway response suspiciously small (%d bytes) — may be fallback",
                session_id[:8],
                len(response_body)
            )

        # Cache gateway response for heartbeats and update token lifecycle
        with self._lock:
            s = self._sessions.get(session_id)
            if s:
                s.gateway_response = response_body
                s.gateway_auth_ok = True
                s.gateway_auth_time = time.time()
                s.token_lifecycle.envelope_issued_at = time.time()
                s.state = SessionState.HEARTBEATING
        
        log.info("[GW] gateway response cached (%d bytes) for heartbeat relay", len(response_body))
        
        token = self.riot.authenticate(container_id, profile)
        
        # Get heartbeat config from global config (matches config.yaml defaults: 10000ms / 500ms)
        from .config import load_config
        from pathlib import Path
        cfg = load_config(Path(__file__).resolve().parent.parent / "config.yaml")
        hb_cfg = cfg.get("heartbeat", {})
        
        interval_ms = int(hb_cfg.get("interval_ms", 10000))
        jitter_max_ms = int(hb_cfg.get("jitter_max_ms", 500))

        log.info(
            "session %s heartbeat config: interval=%dms jitter=%dms",
            session_id[:8],
            interval_ms,
            jitter_max_ms
        )

        scheduler = HeartbeatScheduler(
            session_id, 
            container_id, 
            self.riot,
            interval_ms=interval_ms,
            jitter_max_ms=jitter_max_ms,
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
            # Signal per-session thread to stop
            s._stop_event.set()
            s.state = SessionState.DESTROYED
            s.hb_metrics = HeartbeatMetrics()
            
            # Increment churn rate
            self._purged_last_min += 1

            if s.container_id:
                self.wine.destroy_container(s.container_id)
                self._log_event(session_id, "session", "destroyed", f"cid={s.container_id[:8]}")
                log.info("session %s destroyed", session_id[:8])
            else:
                self._log_event(session_id, "session", "destroyed", "no_container")
                log.info("session %s destroyed (no container)", session_id[:8])
                
            if s._thread and s._thread.is_alive():
                s._thread.join(timeout=0.5)

        # Clean up VGCDriver persistent state
        from .vgc_driver import _driver
        _driver.cleanup_session(session_id)

    def _loop(self) -> None:
        while self._running:
            # Ticking is now handled in per-session threads
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
                # VAL 5 Risk Detection Enhancements
                if s.token_lifecycle.is_f1_expired() or s.token_lifecycle.is_envelope_expired():
                    at_risk.append((sid, "token_expired"))
                    
                hb_status = s.hb_metrics.get_health_status()
                if hb_status in ("warning", "critical"):
                    at_risk.append((sid, f"hb_{hb_status}"))
                    
                time_since_auth = now - s.client_jwt_at if s.client_jwt_at else 999
                time_since_activity = now - s.last_activity
                
                # Risk: No JWT refresh in 4 minutes OR no activity in 3 minutes
                if time_since_auth > 240 or time_since_activity > 180:
                    at_risk.append((sid, "inactive"))
            
            churn_count = self._purged_last_min
            self._purged_last_min = 0  # reset churn tracker
        
        if sessions == 0:
            return
            
        log.info(
            "status: active_sessions=%d containers=%d churn=%d",
            sessions,
            containers,
            churn_count
        )
        
        # Warn about VAL 5 risk
        for sid, reason in at_risk:
            log.warning(
                "session %s VAL 5 RISK: %s",
                sid[:8],
                reason
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
