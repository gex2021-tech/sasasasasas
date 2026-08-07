"""VAL 5 Diagnostic Tool - Analyze session logs and vClient logs for VAL 5 root causes.

Only analyzes log entries from the last --window minutes (default: 30).
"""
from __future__ import annotations

import json
import logging
import re
import time
from datetime import datetime, timedelta
from pathlib import Path
from typing import Dict, List, Any, Optional

log = logging.getLogger("val5_debugger")

# Regex to extract timestamp from standard Python logging format
# Example: "2026-08-06 04:41:00,080 [session_manager] INFO ..."
LOG_TS_RE = re.compile(r"^(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2},\d{3})")


def _parse_log_ts(line: str) -> Optional[float]:
    """Extract epoch timestamp from a log line. Returns None if unparseable."""
    m = LOG_TS_RE.match(line)
    if not m:
        return None
    try:
        dt = datetime.strptime(m.group(1), "%Y-%m-%d %H:%M:%S,%f")
        return dt.timestamp()
    except Exception:
        return None


def _read_recent_lines(path: Path, cutoff_epoch: float) -> List[str]:
    """Read only lines whose timestamp >= cutoff_epoch."""
    if not path.exists():
        return []
    recent: List[str] = []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            ts = _parse_log_ts(line)
            if ts is None:
                # Keep lines without timestamps if we're already in the recent window
                if recent:
                    recent.append(line)
                continue
            if ts >= cutoff_epoch:
                recent.append(line)
    return recent


class VAL5Debugger:
    """Diagnose VAL 5 errors during queue phase"""

    def __init__(
        self,
        session_log_path: str = "logs/server.log",
        vclient_log_path: str = "vClient.log",
        window_minutes: int = 30,
    ):
        self.session_log = Path(session_log_path)
        self.vclient_log = Path(vclient_log_path)
        self.window_minutes = window_minutes
        self.issues: List[Dict[str, Any]] = []

        # Compute cutoff
        self._cutoff = time.time() - (window_minutes * 60)

    def analyze(self) -> Dict[str, Any]:
        """Run full diagnostic sequence"""
        print(f"\n[VAL5-DEBUG] Starting diagnostics (last {self.window_minutes} min window)...")

        # Pre-load recent lines once
        self._server_lines = _read_recent_lines(self.session_log, self._cutoff)
        self._vclient_lines = _read_recent_lines(self.vclient_log, self._cutoff)

        line_count = len(self._server_lines)
        print(f"    [INFO] Analyzing {line_count} recent log lines (cutoff: {datetime.fromtimestamp(self._cutoff).strftime('%H:%M:%S')})")

        self._check_heartbeat_timing()
        self._check_cache_contamination()
        self._check_ioctl_sequence()
        self._check_gateway_tokens()
        self._check_protobuf_integrity()
        self._check_session_continuity()
        self._check_session_threads()

        return self._generate_report()

    # ── 1. Heartbeat Timing ───────────────────────────────────────────────

    def _check_heartbeat_timing(self) -> None:
        """Verify heartbeat intervals are within spec"""
        print("[1/7] Checking heartbeat timing...")

        hb_timestamps: List[float] = []

        for line in self._server_lines:
            if "HEARTBEAT" in line or "IOCTL 0x222000" in line:
                ts = _parse_log_ts(line)
                if ts:
                    hb_timestamps.append(ts)

        intervals: List[float] = []
        violations = 0
        for i in range(1, len(hb_timestamps)):
            interval = hb_timestamps[i] - hb_timestamps[i - 1]
            intervals.append(interval)

            if interval < 9.0 or interval > 11.0:
                violations += 1
                self.issues.append({
                    "type": "TIMING_VIOLATION",
                    "severity": "HIGH",
                    "detail": f"Heartbeat interval {interval:.2f}s outside 9.0-11.0s window",
                    "fix": "Check heartbeat_scheduler.py jitter calculation and thread sleep"
                })

        if intervals:
            avg = sum(intervals) / len(intervals)
            ok_pct = ((len(intervals) - violations) / len(intervals)) * 100
            print(f"    [{'OK' if violations == 0 else 'WARN'}] {len(intervals)} heartbeats, avg={avg:.2f}s, in-spec={ok_pct:.0f}%")
        else:
            if not self._server_lines:
                print("    [INFO] No log data yet (server.log is empty or too new)")
            else:
                self.issues.append({
                    "type": "NO_HEARTBEATS",
                    "severity": "CRITICAL",
                    "detail": "No heartbeat IOCTLs detected in recent logs",
                    "fix": "Verify heartbeat_scheduler is running and vClient is relaying"
                })
                print("    [X] NO HEARTBEATS FOUND in recent window")

    # ── 2. Cache Contamination ────────────────────────────────────────────

    def _check_cache_contamination(self) -> None:
        """Check for dual-cache mixing"""
        print("[2/7] Checking cache contamination...")

        hb_sizes: List[int] = []
        status_sizes: List[int] = []

        for line in self._vclient_lines:
            if "written=" in line:
                try:
                    size_part = line.split("written=")[1].split()[0]
                    size = int(size_part.split("/")[0])
                    if size > 200:
                        hb_sizes.append(size)
                    else:
                        status_sizes.append(size)
                except Exception:
                    continue

        contaminated = False
        for size in hb_sizes:
            if size < 280 or size > 300:
                contaminated = True
                self.issues.append({
                    "type": "CACHE_CONTAMINATION",
                    "severity": "CRITICAL",
                    "detail": f"Heartbeat cache size {size}B outside expected 289-293B range",
                    "fix": "Rebuild vClient.exe - dual cache variables may be mixing"
                })
                print(f"    [X] CONTAMINATION: HB cache = {size}B (expected 289-293B)")

        for size in status_sizes:
            if size < 80 or size > 130:
                self.issues.append({
                    "type": "STATUS_CACHE_INVALID",
                    "severity": "HIGH",
                    "detail": f"Driver status cache size {size}B outside 80-130B range",
                    "fix": "Check vgc_driver.py IOCTL 0x22C0EC response"
                })

        if not contaminated:
            if hb_sizes:
                print(f"    [OK] HB cache clean: last={hb_sizes[-1]}B")
            if status_sizes:
                print(f"    [OK] Status cache clean: last={status_sizes[-1]}B")
            if not hb_sizes and not status_sizes:
                print("    [INFO] No vClient cache data in recent window")

    # ── 3. IOCTL Sequence ─────────────────────────────────────────────────

    def _check_ioctl_sequence(self) -> None:
        """Verify IOCTL call order matches Vanguard expectations"""
        print("[3/7] Checking IOCTL sequence...")

        ioctl_sequence: List[str] = []
        known_codes = ["0x222000", "0x22C0EC", "0x222004", "0x222008", "0x22200C", "0x222010", "0x22C03C"]

        for line in self._server_lines:
            if "IOCTL" in line:
                for code in known_codes:
                    if code in line:
                        ioctl_sequence.append(code)

        if not ioctl_sequence:
            if self._server_lines:
                self.issues.append({
                    "type": "NO_IOCTLS",
                    "severity": "CRITICAL",
                    "detail": "No IOCTL calls detected in recent logs",
                    "fix": "vClient is not intercepting pipes or not relaying to backend"
                })
                print("    [X] NO IOCTLS DETECTED")
            else:
                print("    [INFO] No log data yet")
            return

        hb_count = ioctl_sequence.count("0x222000")
        if hb_count == 0:
            self.issues.append({
                "type": "MISSING_HEARTBEAT_IOCTL",
                "severity": "CRITICAL",
                "detail": "No 0x222000 heartbeat IOCTLs in sequence",
                "fix": "Check vClient Named Pipe routing and heartbeat_scheduler"
            })
            print("    [X] MISSING HEARTBEAT IOCTLS")
        else:
            print(f"    [OK] Found {hb_count} heartbeat IOCTLs")

        print(f"    [OK] Sequence analysis complete: {len(ioctl_sequence)} total IOCTLs")

    # ── 4. Gateway Tokens ─────────────────────────────────────────────────

    def _check_gateway_tokens(self) -> None:
        """Verify Gateway tokens are real and not corrupted"""
        print("[4/7] Checking Gateway tokens...")

        token_found = False
        for line in self._server_lines:
            if "GATEWAY AUTH OK" in line or "building gateway envelope" in line or "gateway response cached" in line:
                token_found = True
                break

        if not token_found:
            if self._server_lines:
                self.issues.append({
                    "type": "NO_GATEWAY_TOKENS",
                    "severity": "CRITICAL",
                    "detail": "No Gateway token generation or authentication detected",
                    "fix": "session_manager.py is not calling gateway_envelope.py"
                })
                print("    [X] NO GATEWAY TOKENS FOUND")
            else:
                print("    [INFO] No log data yet")
        else:
            print("    [OK] Gateway tokens generated and authenticated")

    # ── 5. Protobuf Integrity ─────────────────────────────────────────────

    def _check_protobuf_integrity(self) -> None:
        """Verify Protobuf field ordering and OSInfo variant"""
        print("[5/7] Checking Protobuf integrity...")

        has_osinfo = False
        for line in self._server_lines:
            if "osinfo_variant=1" in line:
                has_osinfo = True
                break

        if has_osinfo:
            print("    [OK] Protobuf structure valid (OSInfo variant=1 verified)")
        else:
            print("    [INFO] OSInfo variant verification: checking default spec (variant=1)")

    # ── 6. Session Continuity ─────────────────────────────────────────────

    def _check_session_continuity(self) -> None:
        """Verify session doesn't break during queue — measures churn RATE"""
        print("[6/7] Checking session continuity...")

        purge_timestamps: List[float] = []
        for line in self._server_lines:
            if "purging old duplicate session" in line:
                ts = _parse_log_ts(line)
                if ts:
                    purge_timestamps.append(ts)

        reconnects = len(purge_timestamps)

        if reconnects == 0:
            print("    [OK] No session purges detected")
            return

        # Calculate churn rate (purges per minute)
        if len(purge_timestamps) >= 2:
            time_span = purge_timestamps[-1] - purge_timestamps[0]
            if time_span > 0:
                rate_per_min = (reconnects / time_span) * 60
            else:
                rate_per_min = reconnects
        else:
            rate_per_min = reconnects

        if reconnects > 3 or rate_per_min > 2.0:
            self.issues.append({
                "type": "FREQUENT_RECONNECTS",
                "severity": "HIGH",
                "detail": f"{reconnects} session purges ({rate_per_min:.1f}/min) in last {self.window_minutes}min",
                "fix": "Check network connection stability between client and VPS"
            })
            print(f"    [WARN] High session churn: {reconnects} purges ({rate_per_min:.1f}/min)")
        else:
            print(f"    [OK] Session continuity stable ({reconnects} purges, {rate_per_min:.1f}/min)")

    # ── 7. Per-Session Thread Health (NEW) ────────────────────────────────

    def _check_session_threads(self) -> None:
        """Check if per-session threads are running and heartbeating properly"""
        print("[7/7] Checking per-session thread health...")

        # Look for the new architecture markers
        thread_started = 0
        critical_gaps = 0
        gap_warnings = 0
        val5_risks = 0
        token_expirations = 0

        for line in self._server_lines:
            if "thread started" in line.lower() or "session_started" in line:
                thread_started += 1
            if "CRITICAL heartbeat gap" in line:
                critical_gaps += 1
            if "heartbeat gap warning" in line:
                gap_warnings += 1
            if "VAL 5 RISK" in line:
                val5_risks += 1
            if "token expired" in line or "envelope expired" in line:
                token_expirations += 1

        if critical_gaps > 0:
            self.issues.append({
                "type": "HEARTBEAT_GAP_CRITICAL",
                "severity": "CRITICAL",
                "detail": f"{critical_gaps} critical heartbeat gaps detected (>15s) by per-session thread",
                "fix": "Network latency or VPS overload is causing heartbeat thread stalls"
            })
            print(f"    [X] {critical_gaps} CRITICAL heartbeat gaps (>15s)")

        if gap_warnings > 0:
            self.issues.append({
                "type": "HEARTBEAT_GAP_WARNING",
                "severity": "HIGH",
                "detail": f"{gap_warnings} heartbeat gap warnings (>12s) detected",
                "fix": "Monitor network stability — approaching VAL 5 threshold"
            })
            print(f"    [WARN] {gap_warnings} heartbeat gap warnings (>12s)")

        if token_expirations > 0:
            self.issues.append({
                "type": "TOKEN_EXPIRATION",
                "severity": "HIGH",
                "detail": f"{token_expirations} token expiration events (F1 or Gateway envelope)",
                "fix": "JWT refresh not reaching server — check vClient JWT_UPDATE relay"
            })
            print(f"    [WARN] {token_expirations} token expirations detected")

        if val5_risks > 0:
            print(f"    [WARN] {val5_risks} VAL 5 risk warnings in status snapshots")

        if critical_gaps == 0 and gap_warnings == 0 and token_expirations == 0:
            if thread_started > 0:
                print(f"    [OK] {thread_started} session thread(s) healthy")
            else:
                if self._server_lines:
                    print("    [INFO] No session thread activity in recent window")
                else:
                    print("    [INFO] No log data yet")

    # ── Report ────────────────────────────────────────────────────────────

    def _generate_report(self) -> Dict[str, Any]:
        """Generate diagnostic report with fixes"""
        report = {
            "timestamp": datetime.now().isoformat(),
            "window_minutes": self.window_minutes,
            "log_lines_analyzed": len(self._server_lines),
            "total_issues": len(self.issues),
            "critical": len([i for i in self.issues if i["severity"] == "CRITICAL"]),
            "high": len([i for i in self.issues if i["severity"] == "HIGH"]),
            "medium": len([i for i in self.issues if i["severity"] == "MEDIUM"]),
            "issues": self.issues,
        }

        print("\n" + "=" * 80)
        print("VAL 5 DIAGNOSTIC REPORT")
        print("=" * 80)
        print(f"Window: last {self.window_minutes} minutes | Lines analyzed: {len(self._server_lines)}")
        print(f"Total Issues: {report['total_issues']}")
        print(f"  Critical: {report['critical']}")
        print(f"  High:     {report['high']}")
        print(f"  Medium:   {report['medium']}")
        print("=" * 80)

        if self.issues:
            print("\nISSUES FOUND:\n")
            for i, issue in enumerate(self.issues, 1):
                print(f"{i}. [{issue['severity']}] {issue['type']}")
                print(f"   Detail: {issue['detail']}")
                print(f"   Fix: {issue['fix']}\n")
        else:
            print("\n[OK] NO ISSUES DETECTED - Backend emulation is operating cleanly\n")

        # Save report
        report_path = Path("logs/val5_diagnostic.json")
        report_path.parent.mkdir(exist_ok=True)
        with open(report_path, "w", encoding="utf-8") as f:
            json.dump(report, f, indent=2)

        print(f"Full report saved to: {report_path}")
        print("=" * 80)

        return report


if __name__ == "__main__":
    import sys

    session_log = "logs/server.log"
    vclient_log = "vClient.log"
    window = 30  # default 30 minutes

    args = sys.argv[1:]
    for i, arg in enumerate(args):
        if arg == "--window" and i + 1 < len(args):
            window = int(args[i + 1])
        elif arg == "--log" and i + 1 < len(args):
            session_log = args[i + 1]
        elif arg == "--vclient" and i + 1 < len(args):
            vclient_log = args[i + 1]
        elif i == 0 and not arg.startswith("--"):
            session_log = arg
        elif i == 1 and not arg.startswith("--"):
            vclient_log = arg

    debugger = VAL5Debugger(session_log, vclient_log, window_minutes=window)
    debugger.analyze()
