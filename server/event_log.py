from __future__ import annotations

import json
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional


@dataclass
class EventRecord:
    type: str
    sequence: int
    timestamp_server: int
    status: str
    latency_ms: Optional[int] = None
    reason: Optional[str] = None
    meta: Optional[Dict[str, Any]] = None

    def to_dict(self) -> Dict[str, Any]:
        out: Dict[str, Any] = {
            "type": self.type,
            "sequence": self.sequence,
            "timestamp_server": self.timestamp_server,
            "status": self.status,
            "latency_ms": self.latency_ms,
            "reason": self.reason,
        }
        if self.meta:
            out["meta"] = self.meta
        return out


class SessionEventLog:
    def __init__(self, base_dir: Path):
        self.base_dir = base_dir
        self.base_dir.mkdir(parents=True, exist_ok=True)

    def log(self, session_id: str, record: EventRecord) -> None:
        path = self.base_dir / f"{session_id}.jsonl"
        with path.open("a", encoding="utf-8") as f:
            f.write(json.dumps(record.to_dict()) + "\n")
