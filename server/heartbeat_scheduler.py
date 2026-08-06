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
    """Cached heartbeat response with metadata."""
    sequence: int
    data: bytes
    sent_at: float = 0.0


class HeartbeatScheduler:
    """Per-session heartbeat dispatcher with anti-cheat stealth jitter.
    
    Implements:
    - Configurable interval + jitter (unpredictable timing vs anti-cheat detection)
    - Sequence numbering + buffering for reconnect sync
    - Per-heartbeat force dispatch (for immediate response to IOCTL)
    - Thread-safe mutation of state (sequence, missed_count, last_sent)
    - Fallback response on error
    """
    
    def __init__(
        self,
        session_id: str,
        container_id: str,
        riot: RiotProxy,
        interval_ms: int = 10000,
        jitter_max_ms: int = 500,
        max_missed: int = 2,
    ):
        self.session_id = session_id
        self.container_id = container_id
        self.riot = riot
        self.interval_ms = interval_ms
        self.jitter_max_ms = jitter_max_ms
        self.max_missed = max_missed
        
        # Sequence state (protected by _lock)
        self.sequence = 0
        self.last_sent = time.time()
        self.last_success = time.time()
        self.missed_count = 0
        
        # Heartbeat buffer for reconnect sync
        self._buffer: Deque[BufferedHeartbeat] = deque(maxlen=256)
        self._lock = threading.Lock()
        
        # Jitter scheduling: compute ONCE at init, then use until next scheduled send
        # This prevents the threshold from drifting mid-interval
        self._next_jitter = random.randint(0, self.jitter_max_ms)
        self._next_send_at = time.time() + (self.interval_ms / 1000.0)

    def tick(self) -> None:
        """Called every 1s by SessionManager._loop(). Check if heartbeat is due."""
        now = time.time()
        
        # Check if it's time to send (interval + jitter)
        if now >= self._next_send_at:
            self.send_heartbeat()
            # Schedule next heartbeat: interval + fresh jitter
            self._next_jitter = random.randint(0, self.jitter_max_ms)
            self._next_send_at = now + (self.interval_ms / 1000.0) + (self._next_jitter / 1000.0)

    def send_heartbeat(
        self,
        force: bool = False,
        ioctl_code: int = 0x222000,
        data: bytes = b"",
    ) -> bytes:
        """Dispatch a heartbeat IOCTL request.
        
        Args:
            force: Send immediately regardless of tick interval (e.g., on IOCTL or first auth).
            ioctl_code: IOCTL opcode (0x222000=heartbeat, 0x22C0EC=driver status, etc).
            data: Optional input payload.
            
        Returns:
            Response bytes (heartbeat token or driver status protobuf).
        """
        with self._lock:
            self.sequence += 1
            seq = self.sequence

        try:
            # Dispatch to Riot proxy (C++ vClient or Wine IOCTL handler)
            resp = self.riot.send_heartbeat(
                self.session_id,
                self.container_id,
                seq,
                ioctl_code,
                data,
            )
            
            # Log based on IOCTL type
            if ioctl_code == 0x222000:
                log.info(
                    "[HB] session %s seq=%d heartbeat_token=%d bytes %s",
                    self.session_id[:8],
                    seq,
                    len(resp),
                    "(force)" if force else "(scheduled)"
                )
            elif (ioctl_code >> 16) == 0x22:
                # Driver status or other 0x22xxxx IOCTL
                log.info(
                    "[IOCTL] session %s seq=%d code=0x%X response=%d bytes",
                    self.session_id[:8],
                    seq,
                    ioctl_code,
                    len(resp)
                )

            # Update state on success
            with self._lock:
                self.missed_count = 0
                self.last_success = time.time()
                self.last_sent = time.time()
                self._buffer.append(BufferedHeartbeat(seq, resp, time.time()))
                
        except Exception as e:
            # Error sending IOCTL — log and use fallback
            log.error(
                "[HB-ERR] session %s seq=%d ioctl=0x%X error: %s",
                self.session_id[:8],
                seq,
                ioctl_code,
                e
            )
            
            with self._lock:
                self.missed_count += 1
                self.last_sent = time.time()
                
                # Fallback: use cached gateway response from session manager
                # or empty bytes if no cache exists
                fallback_resp = self.riot.fallback.get(self.session_id, b"")
                self._buffer.append(BufferedHeartbeat(seq, fallback_resp, time.time()))
                resp = fallback_resp

            # Log missed heartbeat count
            if self.missed_count > self.max_missed:
                log.critical(
                    "[HB-CRITICAL] session %s missed_count=%d (threshold=%d) — VAL 5 risk!",
                    self.session_id[:8],
                    self.missed_count,
                    self.max_missed
                )

        return resp

    def get_range(self, from_seq: int, to_seq: int) -> List[Tuple[int, bytes]]:
        """Retrieve buffered heartbeats for reconnect SYNC (client catch-up).
        
        Args:
            from_seq: Starting sequence number (inclusive).
            to_seq: Ending sequence number (inclusive).
            
        Returns:
            List of (sequence, data) tuples.
        """
        out: List[Tuple[int, bytes]] = []
        with self._lock:
            for hb in self._buffer:
                if from_seq <= hb.sequence <= to_seq:
                    out.append((hb.sequence, hb.data))
        return out

    def seconds_since_success(self) -> float:
        """Time elapsed since last successful heartbeat (for VAL 5 timeout detection)."""
        with self._lock:
            return time.time() - self.last_success

    def get_state_snapshot(self) -> dict:
        """Return scheduler state for logging/debugging."""
        with self._lock:
            return {
                "session_id": self.session_id[:8],
                "sequence": self.sequence,
                "missed_count": self.missed_count,
                "last_success": self.last_success,
                "seconds_since_success": time.time() - self.last_success,
                "buffer_size": len(self._buffer),
                "interval_ms": self.interval_ms,
                "jitter_max_ms": self.jitter_max_ms,
            }


class HeartbeatRelay:
    """Dispatcher for IOCTL requests via per-session HeartbeatScheduler.
    
    Handles:
    - IOCTL routing to scheduler.send_heartbeat()
    - Reconnect SYNC (client catch-up on missed heartbeats)
    - Fallback to Wine/embedded driver if IOCTL is not heartbeat-related
    """
    
    def __init__(self, schedulers: Dict[str, HeartbeatScheduler]):
        self._schedulers = schedulers
        self._lock = threading.Lock()

    def on_ioctl(self, session_id: str, ioctl_code: int, data: bytes) -> bytes:
        """Handle incoming IOCTL request from client (vClient.exe pipe).
        
        Args:
            session_id: Session UUID.
            ioctl_code: IOCTL code (e.g., 0x222000, 0x22C0EC).
            data: Input buffer.
            
        Returns:
            IOCTL response bytes.
        """
        with self._lock:
            sch = self._schedulers.get(session_id)
        
        if not sch:
            log.warning("[IOCTL] session %s not found", session_id[:8])
            return b""

        # Route heartbeat IOCTLs (0x222000 + 0x22xxxx codes)
        if ioctl_code == 0x222000 or (ioctl_code >> 16) == 0x22:
            # Heartbeat or driver-related IOCTL — dispatch via scheduler
            result = sch.send_heartbeat(
                force=True,
                ioctl_code=ioctl_code,
                data=data
            )
            
            # Log compat packets (type 1 = in-game struct per paid emulator logs)
            if ioctl_code in [0x222004, 0x222008, 0x22200C]:
                log.info(
                    "[COMPAT] session %s ioctl=0x%X type=1 in=%d out=%d",
                    session_id[:8],
                    ioctl_code,
                    len(data),
                    len(result)
                )
            
            return result
        else:
            # Non-heartbeat IOCTL — delegate to Wine/embedded driver
            log.debug(
                "[IOCTL-PASSTHROUGH] session %s code=0x%X in=%d",
                session_id[:8],
                ioctl_code,
                len(data)
            )
            try:
                result = sch.riot.wine.send_ioctl(
                    sch.container_id,
                    ioctl_code,
                    data,
                    sch.riot.timeout_ms,
                )
                return result
            except Exception as e:
                log.error(
                    "[IOCTL-ERR] session %s code=0x%X: %s",
                    session_id[:8],
                    ioctl_code,
                    e
                )
                return b""

    def on_reconnect(self, session_id: str, client_last_seq: int) -> List[Tuple[int, bytes]]:
        """Handle SYNC message: client reconnected and needs missed heartbeats.
        
        Args:
            session_id: Session UUID.
            client_last_seq: Last sequence number received by client.
            
        Returns:
            List of (sequence, data) tuples for client to replay.
        """
        with self._lock:
            sch = self._schedulers.get(session_id)
        
        if not sch:
            log.warning("[SYNC] session %s not found", session_id[:8])
            return []

        # Calculate missed range
        missed = sch.sequence - client_last_seq
        if missed <= 0:
            log.info("[SYNC] session %s no missed heartbeats (client_seq=%d server_seq=%d)", 
                     session_id[:8], client_last_seq, sch.sequence)
            return []

        # Retrieve buffered heartbeats in range
        buffered = sch.get_range(client_last_seq + 1, sch.sequence)
        
        if len(buffered) == 0:
            log.warning(
                "[SYNC] session %s requested seq range [%d-%d] but buffer empty or out of range",
                session_id[:8],
                client_last_seq + 1,
                sch.sequence
            )
        else:
            log.info(
                "[SYNC] session %s replaying %d heartbeats (seq %d-%d)",
                session_id[:8],
                len(buffered),
                buffered[0][0],
                buffered[-1][0]
            )

        return buffered

    def get_scheduler_stats(self) -> Dict[str, dict]:
        """Return stats for all active schedulers (for monitoring/logging)."""
        stats = {}
        with self._lock:
            for sid, sch in self._schedulers.items():
                stats[sid] = sch.get_state_snapshot()
        return stats
