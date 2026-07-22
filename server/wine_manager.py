"""Wine/vgc container — program.exe + crypto mount + IOCTL payloads."""
from __future__ import annotations

import logging
import threading
import uuid
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, Optional

from .program_worker import ProgramWorker
from .vgc_crypto import CryptoSession

log = logging.getLogger("wine_manager")


@dataclass
class Container:
    container_id: str
    profile: Dict[str, Any] = field(default_factory=dict)
    crypto: CryptoSession = field(default_factory=CryptoSession)
    worker: Optional[ProgramWorker] = None
    ready: bool = False
    use_program: bool = False


class WineManager:
    def __init__(self, enabled: bool, cfg: Optional[Dict[str, Any]] = None, root: Optional[Path] = None):
        self.enabled = enabled
        self.cfg = cfg or {}
        self.root = root or Path(".")
        self.program_path = Path(self.cfg.get("program_path", "program.exe"))
        if not self.program_path.is_absolute():
            self.program_path = self.root / self.program_path
        self.work_root = Path(self.cfg.get("work_dir", "data/containers"))
        if not self.work_root.is_absolute():
            self.work_root = self.root / self.work_root
        self.ready_timeout_sec = float(self.cfg.get("ready_timeout_sec", 30))
        self.ipc_timeout_ms = int(self.cfg.get("ipc_timeout_ms", 5000))
        self._containers: Dict[str, Container] = {}
        self._lock = threading.Lock()

    def create_container(self) -> str:
        cid = str(uuid.uuid4())
        with self._lock:
            self._containers[cid] = Container(container_id=cid)
        log.info("container %s created (wine=%s program=%s)", cid[:8], self.enabled, self.program_path.name)
        return cid

    def destroy_container(self, container_id: str) -> None:
        with self._lock:
            container = self._containers.pop(container_id, None)
        if not container:
            return
        if container.worker:
            container.worker.stop()
        log.info("destroy container %s", container_id[:8])

    def bind_session_profile(self, container_id: str, profile: Dict[str, Any]) -> None:
        with self._lock:
            container = self._containers.get(container_id)
            if not container:
                return
            container.profile = dict(profile)
        log.info(
            "container %s profile region=%s account=%s hwid=%s",
            container_id[:8],
            profile.get("region", ""),
            str(profile.get("riot_account", ""))[:24],
            str(profile.get("hwid_fingerprint_hex", ""))[:16],
        )

    def get_session_profile(self, container_id: str) -> Dict[str, Any]:
        with self._lock:
            container = self._containers.get(container_id)
            return dict(container.profile) if container else {}

    def wait_ready(self, container_id: str, timeout: float = 30.0) -> bool:
        with self._lock:
            container = self._containers.get(container_id)
            if not container:
                return False
            profile = dict(container.profile)

        container.crypto.mount(profile)

        if self.enabled and self.program_path.exists():
            worker = ProgramWorker(
                container_id,
                self.program_path,
                self.work_root,
                ready_timeout_sec=min(timeout, self.ready_timeout_sec),
                ipc_timeout_ms=self.ipc_timeout_ms,
            )
            if worker.start() and worker.mount_crypto(profile):
                with self._lock:
                    c = self._containers.get(container_id)
                    if c:
                        c.worker = worker
                        c.use_program = True
                        c.ready = True
                log.info("container %s program.exe mounted", container_id[:8])
                return True
            worker.stop()
            log.warning("container %s program.exe failed — using embedded crypto", container_id[:8])

        with self._lock:
            c = self._containers.get(container_id)
            if c:
                c.ready = True
        log.info("container %s ready (embedded crypto)", container_id[:8])
        return True

    def health_check(self, container_id: str) -> bool:
        with self._lock:
            container = self._containers.get(container_id)
            if not container or not container.ready:
                return False
            if container.worker and not container.worker.alive:
                return False
            return True

    def send_ioctl(self, container_id: str, ioctl_code: int, data: bytes, timeout_ms: int) -> bytes:
        with self._lock:
            container = self._containers.get(container_id)
            if not container or not container.ready:
                return b""

        if container.use_program and container.worker and container.worker.alive:
            resp = container.worker.ioctl(ioctl_code, data, timeout_ms)
            if resp:
                log.debug(
                    "ioctl program container=%s code=0x%X in=%d out=%d",
                    container_id[:8],
                    ioctl_code,
                    len(data),
                    len(resp),
                )
                return resp

        resp = container.crypto.ioctl_response(ioctl_code, data)
        log.debug(
            "ioctl crypto container=%s code=0x%X in=%d out=%d",
            container_id[:8],
            ioctl_code,
            len(data),
            len(resp),
        )
        return resp

    def set_client_jwt(self, container_id: str, jwt: str, puuid: str) -> None:
        with self._lock:
            container = self._containers.get(container_id)
            if not container:
                return
            container.profile["jwt"] = jwt
            container.profile["client_puuid"] = puuid
            container.crypto.update_jwt(jwt, puuid)
            worker = container.worker

        if worker and worker.alive:
            worker.set_jwt(jwt, puuid)

        log.info(
            "jwt pushed container=%s puuid=%s len=%d program=%s",
            container_id[:8],
            puuid[:8] if puuid else "",
            len(jwt),
            bool(worker and worker.alive),
        )
