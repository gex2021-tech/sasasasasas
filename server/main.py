from __future__ import annotations

import logging
import signal
import sys
if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8')
if hasattr(sys.stderr, 'reconfigure'):
    sys.stderr.reconfigure(encoding='utf-8')
from datetime import datetime
from pathlib import Path

from .banner import print_startup_banner
from .config import load_config
from .event_log import SessionEventLog
from .fallback_cache import FallbackCache
from .heartbeat_scheduler import HeartbeatRelay, HeartbeatScheduler
from .riot_proxy import RiotProxy
from .session_manager import SessionManager
from .tunnel_server import TunnelServer
from .van84_monitor import Van84Monitor
from .version import PRODUCT_NAME, VERSION
from .wine_manager import WineManager

import atexit
from datetime import datetime
from pathlib import Path

# Setup dual logging (Console + Persistent File logs/server.log)
_log_dir = Path(__file__).resolve().parent.parent / "logs"
_log_dir.mkdir(parents=True, exist_ok=True)
_log_file = _log_dir / "server.log"

_formatter = logging.Formatter("%(asctime)s [%(name)s] %(levelname)s %(message)s")
_file_handler = logging.FileHandler(_log_file, mode="a", encoding="utf-8")
_file_handler.setFormatter(_formatter)
_console_handler = logging.StreamHandler(sys.stdout)
_console_handler.setFormatter(_formatter)

root_logger = logging.getLogger()
root_logger.setLevel(logging.INFO)
root_logger.addHandler(_file_handler)
root_logger.addHandler(_console_handler)

log = logging.getLogger("main")


def main() -> None:
    started_at = datetime.now()
    root = Path(__file__).resolve().parent.parent
    cfg_path = root / "config.yaml"
    if not cfg_path.exists():
        log.error("config.yaml missing — run install.sh")
        sys.exit(1)

    cfg = load_config(cfg_path)
    print_startup_banner(cfg, started_at)

    tunnel = cfg.get("tunnel", {})
    session_cfg = cfg.get("session", {})
    hb_cfg = cfg.get("heartbeat", {})
    paths = cfg.get("paths", {})
    wine_cfg = cfg.get("wine", {})

    auth_key = tunnel.get("auth_key", "")
    if not auth_key or auth_key.startswith("TROQUE"):
        log.error("Defina auth_key em config.yaml")
        sys.exit(1)

    log.info("%s V%s — processo main ativo", PRODUCT_NAME, VERSION)

    event_log = SessionEventLog(root / paths.get("event_log_dir", "logs/sessions"))
    fallback = FallbackCache(root / paths.get("fallback_cache", "data/fallback_cache.json"))
    wine = WineManager(
        enabled=bool(wine_cfg.get("enabled", False)),
        cfg=wine_cfg,
        root=root,
    )
    riot = RiotProxy(
        wine,
        fallback,
        event_log,
        int(hb_cfg.get("riot_timeout_ms", 5000)),
    )

    schedulers: dict[str, HeartbeatScheduler] = {}
    relay = HeartbeatRelay(schedulers)
    session_mgr = SessionManager(
        wine,
        riot,
        schedulers,
        event_log,
        idle_timeout_sec=int(session_cfg.get("idle_timeout_sec", 600)),
    )
    session_mgr.start()

    van84 = Van84Monitor(schedulers, int(hb_cfg.get("van84_threshold_sec", 420)))
    van84.start()

    cert = root / tunnel.get("tls_cert", "certs/server.pem")
    key = root / tunnel.get("tls_key", "certs/server.key")
    if not cert.exists() or not key.exists():
        log.error("TLS certs missing — run install.sh")
        sys.exit(1)

    server = TunnelServer(
        host=tunnel.get("host", "0.0.0.0"),
        port=int(tunnel.get("port", 51820)),
        auth_key=auth_key,
        tls_cert=cert,
        tls_key=key,
        session_mgr=session_mgr,
        relay=relay,
        max_clients=int(tunnel.get("max_clients", 32)),
    )

    def shutdown(*_):
        log.info("%s V%s shutdown initiated by signal — saving active state and closing sessions", PRODUCT_NAME, VERSION)
        try:
            session_mgr.stop()
            van84.stop()
        except Exception as e:
            log.warning("Error during session cleanup: %s", e)
        log.info("Server stopped cleanly.")
        sys.exit(0)

    signal.signal(signal.SIGINT, shutdown)
    signal.signal(signal.SIGTERM, shutdown)

    try:
        server.serve_forever()
    except Exception as e:
        log.critical("Unhandled server exception: %s", e, exc_info=True)
        shutdown()


if __name__ == "__main__":
    main()
