"""IPC worker for program.exe — mount, crypto, IOCTL relay."""
from __future__ import annotations

import json
import logging
import socket
import struct
import subprocess
import threading
import time
from pathlib import Path
from typing import Any, Dict, Optional

log = logging.getLogger("program_worker")

_CMD_MOUNT = 1
_CMD_IOCTL = 2
_CMD_SET_JWT = 3
_CMD_PING = 4
_CMD_SHUTDOWN = 5

_REQ = struct.Struct("!BBIII")
_RESP = struct.Struct("!II")


class ProgramWorker:
    """One program.exe instance per container."""

    def __init__(
        self,
        container_id: str,
        program_path: Path,
        work_root: Path,
        ready_timeout_sec: float = 30.0,
        ipc_timeout_ms: int = 5000,
    ):
        self.container_id = container_id
        self.program_path = program_path
        self.work_root = work_root
        self.ready_timeout_sec = ready_timeout_sec
        self.ipc_timeout_ms = ipc_timeout_ms
        self.work_dir = work_root / container_id
        self.port = 0
        self._proc: Optional[subprocess.Popen] = None
        self._lock = threading.Lock()
        self._ready = False

    @property
    def alive(self) -> bool:
        return self._proc is not None and self._proc.poll() is None

    def start(self) -> bool:
        self.work_dir.mkdir(parents=True, exist_ok=True)
        if not self.program_path.exists():
            log.error("program.exe missing: %s", self.program_path)
            return False

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind(("127.0.0.1", 0))
            self.port = s.getsockname()[1]

        cmd = [
            str(self.program_path),
            "--container",
            self.container_id,
            "--ipc-port",
            str(self.port),
            "--work-dir",
            str(self.work_dir),
        ]
        try:
            self._proc = subprocess.Popen(
                cmd,
                cwd=str(self.work_dir),
                stdout=subprocess.DEVNULL,
                stderr=subprocess.PIPE,
            )
        except OSError as exc:
            log.error("failed to spawn program.exe: %s", exc)
            return False

        deadline = time.time() + self.ready_timeout_sec
        while time.time() < deadline:
            if self._proc.poll() is not None:
                err = (self._proc.stderr.read() if self._proc.stderr else b"").decode("utf-8", errors="replace")
                log.error("program.exe exited early (%s): %s", self.container_id[:8], err[:400])
                return False
            if self._ping():
                self._ready = True
                log.info("program.exe ready container=%s port=%d", self.container_id[:8], self.port)
                return True
            time.sleep(0.15)

        log.error("program.exe ready timeout container=%s", self.container_id[:8])
        self.stop()
        return False

    def stop(self) -> None:
        with self._lock:
            if self._proc and self.alive:
                try:
                    self._request(_CMD_SHUTDOWN, 0, b"", timeout_ms=1000)
                except OSError:
                    pass
                self._proc.terminate()
                try:
                    self._proc.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    self._proc.kill()
            self._proc = None
            self._ready = False

    def mount_crypto(self, profile: Dict[str, Any]) -> bool:
        payload = json.dumps(profile, default=str).encode("utf-8")
        status, data = self._request(_CMD_MOUNT, 0, payload)
        if status != 0:
            log.error("mount failed container=%s status=%d msg=%s", self.container_id[:8], status, data[:120])
            return False
        log.info("program mount ok container=%s out=%d", self.container_id[:8], len(data))
        return True

    def set_jwt(self, jwt: str, puuid: str) -> None:
        body = json.dumps({"jwt": jwt, "puuid": puuid}).encode("utf-8")
        self._request(_CMD_SET_JWT, 0, body)

    def ioctl(self, ioctl_code: int, data: bytes, timeout_ms: int) -> bytes:
        status, resp = self._request(_CMD_IOCTL, ioctl_code, data, timeout_ms=timeout_ms)
        if status != 0:
            log.warning(
                "program ioctl err container=%s code=0x%X status=%d",
                self.container_id[:8],
                ioctl_code,
                status,
            )
            return b""
        return resp

    def _ping(self) -> bool:
        try:
            status, _ = self._request(_CMD_PING, 0, b"", timeout_ms=500)
            return status == 0
        except OSError:
            return False

    def _request(self, cmd: int, ioctl_code: int, data: bytes, timeout_ms: Optional[int] = None) -> tuple[int, bytes]:
        if not self.port:
            raise OSError("program worker not started")
        ms = timeout_ms if timeout_ms is not None else self.ipc_timeout_ms
        req = _REQ.pack(1, cmd, 0, ioctl_code, len(data)) + data
        with socket.create_connection(("127.0.0.1", self.port), timeout=ms / 1000.0) as sock:
            sock.settimeout(ms / 1000.0)
            sock.sendall(req)
            hdr = self._recv_exact(sock, _RESP.size)
            status, dlen = _RESP.unpack(hdr)
            body = self._recv_exact(sock, dlen) if dlen else b""
            return status, body

    @staticmethod
    def _recv_exact(sock: socket.socket, size: int) -> bytes:
        buf = bytearray()
        while len(buf) < size:
            chunk = sock.recv(size - len(buf))
            if not chunk:
                raise OSError("short read from program.exe")
            buf.extend(chunk)
        return bytes(buf)
