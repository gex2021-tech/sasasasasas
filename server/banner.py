from __future__ import annotations

import socket
from datetime import datetime
from typing import Any


def _local_ip() -> str:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.connect(("8.8.8.8", 80))
            return s.getsockname()[0]
    except OSError:
        return "127.0.0.1"


def print_startup_banner(cfg: dict[str, Any], started_at: datetime) -> None:
    tunnel = cfg.get("tunnel", {})
    session_cfg = cfg.get("session", {})
    hb_cfg = cfg.get("heartbeat", {})
    wine_cfg = cfg.get("wine", {})
    auth_key = str(tunnel.get("auth_key", ""))

    host = tunnel.get("host", "0.0.0.0")
    port = int(tunnel.get("port", 51820))
    wine_on = bool(wine_cfg.get("enabled", False))
    idle = int(session_cfg.get("idle_timeout_sec", 600))
    van84 = int(hb_cfg.get("van84_threshold_sec", 420))
    hb_ms = int(hb_cfg.get("interval_ms", 30000))

    started_str = started_at.strftime("%Y-%m-%d %H:%M:%S %Z").strip()
    if not started_at.tzinfo:
        started_str = started_at.strftime("%Y-%m-%d %H:%M:%S")

    ip = _local_ip()
    key_hint = (auth_key[:12] + "...") if len(auth_key) > 12 else auth_key

    lines = [
        "",
        "  ____            ___     __",
        " |  _ \\  _ __   |   \\    \\ \\   / __ _  _ __   __ _  _  ___",
        " | | | || '_ \\  | |) |    \\ \\ / / _` || '_ \\ / _` || |/ __|",
        " | |_| || | | | |___/      \\ V / (_| || | | | (_| || |\\__ \\",
        " |____/ |_| |_|              \\_/ \\__,_||_| |_|\\__, ||_||___/",
        "                                              |___/",
        "",
        "  DndVanguard Session Manager  V0.4",
        f"  Iniciado na VM: {started_str}",
        "",
        "  ── Tunnel ─────────────────────────────────────",
        f"  Listen     : {host}:{port}  (TLS)",
        f"  VM IP      : {ip}",
        f"  auth_key   : {key_hint}",
        f"  max_clients: {tunnel.get('max_clients', 32)}",
        "",
        "  ── Sessão / plano híbrido ─────────────────────",
        f"  idle_timeout     : {idle}s",
        f"  heartbeat VM     : {hb_ms}ms (scheduler após pipe AUTH)",
        f"  VAN-84 threshold : {van84}s",
        f"  Wine containers  : {'ON (real vgc)' if wine_on else 'STUB (wine.enabled=false)'}",
        "",
        "  Fluxo cliente → VM:",
        "    SESSION_AUTH → CRIA sessão + container + JWT (pipe 0x64)",
        "    JWT_UPDATE   → refresh (só após sessão ativa)",
        "    IOCTL/PING   → relay (após sessão ativa)",
        "    HELLO/JWT antigos → rejeitados",
        "",
        "  Aguardando clientes...",
        "",
    ]
    print("\n".join(lines), flush=True)
