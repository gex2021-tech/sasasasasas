from __future__ import annotations

import json
import threading
import time
from pathlib import Path
from typing import Dict, Optional


class FallbackCache:
    TTL_SEC = 86400

    def __init__(self, path: Path):
        self.path = path
        self._lock = threading.Lock()
        self._data: Dict[str, Dict[str, object]] = {}
        self._load()

    def _load(self) -> None:
        if not self.path.exists():
            return
        try:
            raw = json.loads(self.path.read_text(encoding="utf-8"))
            self._data = raw if isinstance(raw, dict) else {}
        except (json.JSONDecodeError, OSError):
            self._data = {}

    def _save(self) -> None:
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self.path.write_text(json.dumps(self._data), encoding="utf-8")

    def update(self, session_id: str, response: bytes) -> None:
        with self._lock:
            self._data[session_id] = {"ts": time.time(), "hex": response.hex()}
            self._save()

    def get(self, session_id: str) -> Optional[bytes]:
        with self._lock:
            entry = self._data.get(session_id)
            if not entry:
                return None
            if time.time() - float(entry["ts"]) > self.TTL_SEC:
                return None
            return bytes.fromhex(str(entry["hex"]))
