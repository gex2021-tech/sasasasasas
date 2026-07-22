from __future__ import annotations

import logging
import time

from .event_log import EventRecord, SessionEventLog
from .fallback_cache import FallbackCache
from .wine_manager import WineManager

log = logging.getLogger("riot_proxy")


class RiotProxy:
    def __init__(self, wine: WineManager, fallback: FallbackCache, event_log: SessionEventLog, timeout_ms: int):
        self.wine = wine
        self.fallback = fallback
        self.event_log = event_log
        self.timeout_ms = timeout_ms

    def send_heartbeat(
        self,
        session_id: str,
        container_id: str,
        sequence: int,
        ioctl_code: int = 0x222000,
        data: bytes = b"",
    ) -> bytes:
        t0 = time.time()
        resp = self.wine.send_ioctl(container_id, ioctl_code, data, self.timeout_ms)
        self.fallback.update(session_id, resp)
        self.event_log.log(
            session_id,
            EventRecord(
                type="heartbeat",
                sequence=sequence,
                timestamp_server=int(time.time() * 1000),
                status="ok" if resp else "empty",
                latency_ms=int((time.time() - t0) * 1000),
            ),
        )
        return resp

    def authenticate(self, container_id: str, profile: dict | None = None) -> str:
        profile = profile or self.wine.get_session_profile(container_id)
        region = profile.get("region", "")
        account = profile.get("riot_account", "")
        hwid = profile.get("hwid_fingerprint_hex", "")
        log.info(
            "riot_proxy authenticate stub container=%s region=%s account=%s hwid=%s",
            container_id[:8],
            region,
            str(account)[:24],
            str(hwid)[:16],
        )
        token = profile.get("jwt", "")
        if token:
            return token
        return "lab_stub_token"

    def on_client_jwt(self, session_id: str, container_id: str, jwt: str, puuid: str) -> None:
        if not jwt:
            return
        self.wine.set_client_jwt(container_id, jwt, puuid)
        self.event_log.log(
            session_id,
            EventRecord(
                type="jwt_update",
                sequence=0,
                timestamp_server=int(time.time() * 1000),
                status="ok",
                reason=f"puuid={puuid[:8]}" if puuid else None,
            ),
        )
        log.info(
            "riot_proxy jwt session=%s container=%s puuid=%s",
            session_id[:8],
            container_id[:8],
            puuid[:8] if puuid else "",
        )
