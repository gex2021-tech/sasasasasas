from __future__ import annotations

import logging
import random
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
        # Magic numbers from paid emulator logs
        self.magic_auth = 0x66  # Used in auth/gateway
        self.magic_ingame = 0x67  # Used in type 1 packets during game

    def tick(self) -> None:
        elapsed_ms = (time.time() - self.last_sent) * 1000
        # Per-tick random jitter — each check uses a fresh offset
        # so heartbeat timing is unpredictable (stealth)
        jitter = random.randint(-self.jitter_max_ms, self.jitter_max_ms)
        threshold = self.interval_ms + jitter
        if elapsed_ms >= threshold:
            self.send_heartbeat()

    def send_heartbeat(
        self,
        force: bool = False,
        ioctl_code: int = 0x222000,
        data: bytes = b"",
    ) -> bytes:
        """Dispatch a heartbeat IOCTL ping/payload.
        
        Args:
            force: Signals immediate/on-demand heartbeat dispatch regardless of tick interval.
            ioctl_code: IOCTL opcode (0x222000 default or 0x22C0EC/0x222004 DeviceType 0x22).
            data: Payload input buffer.
        """
        with self._lock:
            self.sequence += 1
            seq = self.sequence

        try:
            resp = self.riot.send_heartbeat(
                self.session_id,
                self.container_id,
                seq,
                ioctl_code,
                data,
            )
            if ioctl_code == 0x222000:
                log.info(f"[PIPE][HB] vgk ping ack #{seq} written={len(resp)}/{len(resp)}")
            elif (ioctl_code >> 16) == 0x22:
                log.info(f"[PIPE] packet#{seq} {len(resp)} bytes (0x{ioctl_code & 0xFFFF:x})")

            with self._lock:
                self.missed_count = 0
                self.last_success = time.time()
                self.last_sent = time.time()
                self._buffer.append(BufferedHeartbeat(seq, resp))
        except Exception as e:
            log.exception("hb error: %s", e)
            with self._lock:
                self.missed_count += 1
                self.last_sent = time.time()
            resp = self.riot.fallback.get(self.session_id) or b""
            with self._lock:
                self._buffer.append(BufferedHeartbeat(seq, resp))

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
            result = sch.send_heartbeat(force=True, ioctl_code=ioctl_code, data=data)
            # Log compat packets like paid emulator
            if ioctl_code in [0x222004, 0x222008, 0x22200C]:  # Type 1 packets
                log.info(f"[PIPE][COMPAT] struct type=1 magic=0x{sch.magic_ingame:02X} bytes={len(data)}")
                log.info(f"[PIPE][COMPAT] type 1 echo ACK magic=0x{sch.magic_auth:02X}")
                log.info(f"[PIPE][COMPAT] type 1 reply written={len(result)}/{len(result)}")
            return result
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
