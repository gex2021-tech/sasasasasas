from __future__ import annotations

import logging
import threading
import time
from typing import Dict

from .heartbeat_scheduler import HeartbeatScheduler

log = logging.getLogger("van84")


class Van84Monitor:
    def __init__(self, schedulers: Dict[str, HeartbeatScheduler], threshold_sec: int):
        self._schedulers = schedulers
        self.threshold_sec = threshold_sec
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._running = False

    def start(self) -> None:
        self._running = True
        self._thread.start()

    def stop(self) -> None:
        self._running = False

    def _loop(self) -> None:
        while self._running:
            for sid, sch in list(self._schedulers.items()):
                elapsed = sch.seconds_since_success()
                if elapsed > self.threshold_sec:
                    log.critical("VAN84 preventivo %s após %.0fs", sid[:8], elapsed)
                    sch.send_heartbeat(force=True)
            time.sleep(5.0)
