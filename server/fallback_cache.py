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
        try:
            if not self.path.exists():
                return
            content = self.path.read_text(encoding="utf-8")
            raw = json.loads(content)
            self._data = raw if isinstance(raw, dict) else {}
        except Exception:
            self._data = {}

    def _save(self) -> None:
        try:
            self.path.parent.mkdir(parents=True, exist_ok=True)
            # Use atomic write via temp file or direct write with error handling
            with open(str(self.path), "w", encoding="utf-8") as f:
                json.dump(self._data, f)
        except Exception:
            # Non-fatal: in-memory cache remains active even if disk persistence fails
            pass

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
            try:
                return bytes.fromhex(str(entry["hex"]))
            except ValueError:
                return None

