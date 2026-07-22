from __future__ import annotations

import logging
import threading
import time
from collections import deque
from dataclasses import dataclass
from typing import Deque, Dict, List, Optional, Tuple

from .riot_proxy import RiotProxy

log = logging.getLogger("heartbeat")


@dataclass
class BufferedHeartbeat:
    sequence: int
    data: bytes


class HeartbeatScheduler:
    def __init__(
        self,
        session_id: str,
        container_id: str,
        riot: RiotProxy,
        interval_ms: int = 30000,
        jitter_max_ms: int = 500,
        max_missed: int = 2,
    ):
        self.session_id = session_id
        self.container_id = container_id
        self.riot = riot
        self.interval_ms = interval_ms
        self.jitter_max_ms = jitter_max_ms
        self.max_missed = max_missed
        self.sequence = 0
        self.last_sent = time.time()
        self.last_success = time.time()
        self.missed_count = 0
        self._buffer: Deque[BufferedHeartbeat] = deque(maxlen=256)
        self._lock = threading.Lock()

    def tick(self) -> None:
        elapsed_ms = (time.time() - self.last_sent) * 1000
        if elapsed_ms >= self.interval_ms - self.jitter_max_ms:
            self.send_heartbeat()

    def send_heartbeat(
        self,
        force: bool = False,
        ioctl_code: int = 0x222000,
        data: bytes = b"",
    ) -> bytes:
        self.sequence += 1
        try:
            resp = self.riot.send_heartbeat(
                self.session_id,
                self.container_id,
                self.sequence,
                ioctl_code,
                data,
            )
            self.missed_count = 0
            self.last_success = time.time()
        except Exception as e:
            log.exception("hb error: %s", e)
            self.missed_count += 1
            resp = self.riot.fallback.get(self.session_id) or b""
        self._record_heartbeat(resp)
        self.last_sent = time.time()
        if self.missed_count > self.max_missed:
            log.critical("session %s: missed HB risk Error 102", self.session_id[:8])
        return resp

    def _record_heartbeat(self, resp: bytes) -> None:
        with self._lock:
            self._buffer.append(BufferedHeartbeat(self.sequence, resp))

    def get_range(self, from_seq: int, to_seq: int) -> List[Tuple[int, bytes]]:
        out: List[Tuple[int, bytes]] = []
        with self._lock:
            for hb in self._buffer:
                if from_seq <= hb.sequence <= to_seq:
                    out.append((hb.sequence, hb.data))
        return out

    def seconds_since_success(self) -> float:
        return time.time() - self.last_success


class HeartbeatRelay:
    def __init__(self, schedulers: Dict[str, HeartbeatScheduler]):
        self._schedulers = schedulers

    def on_ioctl(self, session_id: str, ioctl_code: int, data: bytes) -> bytes:
        sch = self._schedulers.get(session_id)
        if not sch:
            return b""
        # Real vgk: DeviceType 0x22 (0x22C000–0x22C17C). Stub: 0x222000.
        if (ioctl_code >> 16) == 0x22 or ioctl_code == 0x222000:
            return sch.send_heartbeat(force=True, ioctl_code=ioctl_code, data=data)
        return sch.riot.wine.send_ioctl(
            sch.container_id,
            ioctl_code,
            data,
            sch.riot.timeout_ms,
        )

    def on_reconnect(self, session_id: str, client_last_seq: int) -> List[Tuple[int, bytes]]:
        sch = self._schedulers.get(session_id)
        if not sch:
            return []
        missed = sch.sequence - client_last_seq
        if missed <= 0:
            return []
        buffered = sch.get_range(client_last_seq + 1, sch.sequence)
        log.info("SYNC %s: %d heartbeats", session_id[:8], len(buffered))
        return buffered
