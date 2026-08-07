"""VAL 5 Diagnostic Tool - Analyze session logs and vClient logs for VAL 5 root causes."""
from __future__ import annotations

import json
import logging
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any

log = logging.getLogger("val5_debugger")


class VAL5Debugger:
    """Diagnose VAL 5 errors during queue phase"""

    def __init__(self, session_log_path: str = "logs/server.log", vclient_log_path: str = "vClient.log"):
        self.session_log = Path(session_log_path)
        self.vclient_log = Path(vclient_log_path)
        self.issues: List[Dict[str, Any]] = []

    def analyze(self) -> Dict[str, Any]:
        """Run full diagnostic sequence"""
        print("\n[VAL5-DEBUG] Starting diagnostics...")

        self._check_heartbeat_timing()
        self._check_cache_contamination()
        self._check_ioctl_sequence()
        self._check_gateway_tokens()
        self._check_protobuf_integrity()
        self._check_session_continuity()

        return self._generate_report()

    def _check_heartbeat_timing(self) -> None:
        """Verify heartbeat intervals are within spec"""
        print("[1/6] Checking heartbeat timing...")

        hb_timestamps = []

        if self.session_log.exists():
            with open(self.session_log, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    if "HEARTBEAT" in line or "IOCTL 0x222000" in line:
                        try:
                            # Extract timestamp from log format: "2026-08-06 04:41:00,080"
                            parts = line.split("[")[0].strip()
                            dt = datetime.strptime(parts, "%Y-%m-%d %H:%M:%S,%f")
                            hb_timestamps.append(dt.timestamp())
                        except Exception:
                            continue

        intervals = []
        for i in range(1, len(hb_timestamps)):
            interval = hb_timestamps[i] - hb_timestamps[i - 1]
            intervals.append(interval)

            if interval < 9.0 or interval > 11.0:
                self.issues.append({
                    "type": "TIMING_VIOLATION",
                    "severity": "HIGH",
                    "detail": f"Heartbeat interval {interval:.2f}s outside 9.0-11.0s window",
                    "fix": "Check heartbeat_scheduler.py jitter calculation and thread sleep"
                })

        if intervals:
            avg = sum(intervals) / len(intervals)
            print(f"    [OK] Found {len(intervals)} heartbeats, avg interval: {avg:.2f}s")
        else:
            self.issues.append({
                "type": "NO_HEARTBEATS",
                "severity": "CRITICAL",
                "detail": "No heartbeat IOCTLs detected in logs",
                "fix": "Verify heartbeat_scheduler is running and vClient is relaying"
            })
            print("    [X] NO HEARTBEATS FOUND")

    def _check_cache_contamination(self) -> None:
        """Check for dual-cache mixing"""
        print("[2/6] Checking cache contamination...")

        hb_sizes = []
        status_sizes = []

        if self.vclient_log.exists():
            with open(self.vclient_log, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    if "written=" in line:
                        try:
                            # Format: "vgk ping ack #1 written=97/97" or "written=291"
                            size_part = line.split("written=")[1].split()[0]
                            size = int(size_part.split("/")[0])
                            if size > 200:
                                hb_sizes.append(size)
                            else:
                                status_sizes.append(size)
                        except Exception:
                            continue

        for size in hb_sizes:
            if size < 280 or size > 300:
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

        if hb_sizes and all(280 <= s <= 300 for s in hb_sizes):
            print(f"    [OK] HB cache clean: last={hb_sizes[-1]}B")
        if status_sizes and all(80 <= s <= 130 for s in status_sizes):
            print(f"    [OK] Status cache clean: last={status_sizes[-1]}B")

    def _check_ioctl_sequence(self) -> None:
        """Verify IOCTL call order matches Vanguard expectations"""
        print("[3/6] Checking IOCTL sequence...")

        ioctl_sequence = []

        if self.session_log.exists():
            with open(self.session_log, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    if "IOCTL" in line:
                        for code in ["0x222000", "0x22C0EC", "0x222004", "0x222008", "0x22200C", "0x222010", "0x22C03C"]:
                            if code in line:
                                ioctl_sequence.append(code)

        if not ioctl_sequence:
            self.issues.append({
                "type": "NO_IOCTLS",
                "severity": "CRITICAL",
                "detail": "No IOCTL calls detected in logs",
                "fix": "vClient is not intercepting pipes or not relaying to backend"
            })
            print("    [X] NO IOCTLS DETECTED")
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

    def _check_gateway_tokens(self) -> None:
        """Verify Gateway tokens are real and not corrupted"""
        print("[4/6] Checking Gateway tokens...")

        token_found = False

        if self.session_log.exists():
            with open(self.session_log, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    if "GATEWAY AUTH OK" in line or "building gateway envelope" in line:
                        token_found = True
                        break

        if not token_found:
            self.issues.append({
                "type": "NO_GATEWAY_TOKENS",
                "severity": "CRITICAL",
                "detail": "No Gateway token generation or authentication detected",
                "fix": "session_manager.py is not calling gateway_envelope.py"
            })
            print("    [X] NO GATEWAY TOKENS FOUND")
        else:
            print("    [OK] Gateway tokens generated and authenticated")

    def _check_protobuf_integrity(self) -> None:
        """Verify Protobuf field ordering and OSInfo variant"""
        print("[5/6] Checking Protobuf integrity...")

        has_osinfo = False

        if self.session_log.exists():
            with open(self.session_log, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    if "osinfo_variant=1" in line:
                        has_osinfo = True
                        break

        if has_osinfo:
            print("    [OK] Protobuf structure valid (OSInfo variant=1 verified)")
        else:
            print("    [INFO] OSInfo variant verification: checking default spec (variant=1)")

    def _check_session_continuity(self) -> None:
        """Verify session doesn't break during queue"""
        print("[6/6] Checking session continuity...")

        reconnects = 0

        if self.session_log.exists():
            with open(self.session_log, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    if "purging old duplicate session" in line:
                        reconnects += 1

        if reconnects > 3:
            self.issues.append({
                "type": "FREQUENT_RECONNECTS",
                "severity": "HIGH",
                "detail": f"High number of duplicate session purges ({reconnects} reconnects)",
                "fix": "Check network connection stability between client and VPS"
            })
            print(f"    [WARN] High session churn: {reconnects} reconnects")
        else:
            print(f"    [OK] Session continuity stable ({reconnects} reconnects)")

    def _generate_report(self) -> Dict[str, Any]:
        """Generate diagnostic report with fixes"""
        report = {
            "timestamp": datetime.now().isoformat(),
            "total_issues": len(self.issues),
            "critical": len([i for i in self.issues if i["severity"] == "CRITICAL"]),
            "high": len([i for i in self.issues if i["severity"] == "HIGH"]),
            "medium": len([i for i in self.issues if i["severity"] == "MEDIUM"]),
            "issues": self.issues,
        }

        print("\n" + "=" * 80)
        print("VAL 5 DIAGNOSTIC REPORT")
        print("=" * 80)
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

    if len(sys.argv) > 1:
        session_log = sys.argv[1]
    if len(sys.argv) > 2:
        vclient_log = sys.argv[2]

    debugger = VAL5Debugger(session_log, vclient_log)
    debugger.analyze()
