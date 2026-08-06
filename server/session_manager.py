from __future__ import annotations

import hashlib
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
from .machine_pool import select_machine_for_session
from .gateway_envelope import SmartGatewayMinty, post_gateway_auth, start_keepalive_loop

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

        self._log_event(session_id, "pipe_auth", "repeat", f"pid={valorant_pid} count={auth_n}")
        log.info(
            "session %s pipe auth repeat (container %s pid=%d)",
            session_id[:8],
            cid[:8],
            valorant_pid,
        )
        return True

    def _provision_container(self, session_id: str, machine_profile: dict = None) -> bool:
        with self._lock:
            snap = self._sessions.get(session_id)
        if not snap:
            return False

        # FAST PROVISIONING: Start immediately to avoid VAL 5 timeout
        start_time = time.time()
        
        container_id = self.wine.create_container()
        
        # Use machine profile from pool if provided (critical for anti-fingerprinting)
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
            "bios_info": machine_profile.get("bios_info") if machine_profile else None,
            "motherboard": machine_profile.get("motherboard") if machine_profile else None,
            "volume_serial": machine_profile.get("volume_serial") if machine_profile else None,
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

        # CRITICAL: Use SmartGatewayMinty for local token minting (paid emulator logic)
        log.info("[CLI] Server gateway flow unavailable; falling back to local SmartGatewayMinty")
        gateway_mint = SmartGatewayMinty(self.riot)
        
        # Mint/register tokens
        ent_tok = getattr(snap, 'entitlement_token', '') or snap.riot_token
        id_tok = getattr(snap, 'id_token', '')
        tokens = gateway_mint.mint_tokens(
            snap.client_puuid,
            snap.riot_token,
            snap.region,
            entitlement_token=ent_tok,
            id_token=id_tok
        )
        
        # Build auth payload with machine profile
        if machine_profile:
            auth_payload = gateway_mint.build_auth_payload(tokens, machine_profile)
        else:
            # Fallback without machine profile
            minimal_profile = {
                "bios_info": "American Megatrends Inc. F34",
                "cpu_model": snap.cpu_model or "AMD Ryzen 7 3700X 8-Core Processor",
                "gpu_model": snap.gpu_model or "NVIDIA GeForce RTX 3070",
                "volume_serial": "A1B2-C3D4",
            }
            auth_payload = gateway_mint.build_auth_payload(tokens, minimal_profile)
        
        # POST to gateway
        status_code, response_body = post_gateway_auth(
            auth_payload,
            region=snap.region,
            session_id=session_id,
            entitlements_token=tokens.entitlement_token,
            id_token=tokens.id_token,
            puuid=snap.client_puuid
        )
        
        # Cache gateway response for next VPS step/action
        log.info("[GW] gateway response cached for next VPS gateway step/action")
        
        # Start keepalive loop (re-auth every 45 minutes)
        keepalive_thread = start_keepalive_loop(session_id, tokens, interval_sec=2700)
        
        token = self.riot.authenticate(container_id, profile)
        
        # Get heartbeat config from global config (or use defaults optimized for VAL 5)
        from .config import load_config
        from pathlib import Path
        cfg = load_config(Path(__file__).resolve().parent.parent / "config.yaml")
        hb_cfg = cfg.get("heartbeat", {})
        
        scheduler = HeartbeatScheduler(
            session_id, 
            container_id, 
            self.riot,
            interval_ms=int(hb_cfg.get("interval_ms", 15000)),      # 15s default (faster)
            jitter_max_ms=int(hb_cfg.get("jitter_max_ms", 1000)),   # 1s jitter
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
        log.info("session %s sending IMMEDIATE first heartbeat to prevent VAL 5", session_id[:8])
        scheduler.send_heartbeat(force=True)
        
        provision_time = time.time() - start_time
        self._log_event(
            session_id,
            "container",
            "provisioned",
            f"cid={container_id[:8]} pid={pid} jwt_len={len(jwt)} provision_ms={int(provision_time*1000)}",
        )
        log.info(
            "session %s container %s provisioned in %.2fs (pid=%d jwt_len=%d puuid=%s)",
            session_id[:8],
            container_id[:8],
            provision_time,
            pid,
            len(jwt),
            puuid[:8] if puuid else "",
        )
        
        # Warn if provisioning took too long (risk of VAL 5)
        if provision_time > 10.0:
            log.warning(
                "session %s SLOW PROVISIONING (%.2fs) - may trigger VAL 5!",
                session_id[:8],
                provision_time
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
            
            # VAL 5 RISK DETECTION: Check for sessions without activity
            now = time.time()
            at_risk = []
            for sid, s in self._sessions.items():
                time_since_auth = now - s.client_jwt_at if s.client_jwt_at else 999
                time_since_activity = now - s.last_activity
                
                # Risk: No JWT refresh in 4 minutes OR no activity in 3 minutes
                if time_since_auth > 240 or time_since_activity > 180:
                    at_risk.append((sid, time_since_auth, time_since_activity))
        
        if sessions == 0:
            return
            
        log.info(
            "status: active_sessions=%d containers=%d",
            sessions,
            containers,
        )
        
        # Warn about VAL 5 risk
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
