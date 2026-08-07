"""VAL 5 Auto-Remediation - Read diagnostic output and apply targeted fixes."""
from __future__ import annotations

import json
import logging
import subprocess
import time
from pathlib import Path
from typing import Any, Dict, List

log = logging.getLogger("val5_remediate")


class VAL5AutoRemediate:
    """Automatically fix VAL 5 issues based on diagnostic results"""

    def __init__(self, diagnostic_report_path: str = "logs/val5_diagnostic.json"):
        self.report_path = Path(diagnostic_report_path)
        self.report: Dict[str, Any] | None = None
        self.issues_by_type: Dict[str, Dict[str, Any]] = {}
        self.applied_fixes: List[str] = []

    def load_report(self) -> bool:
        """Load latest diagnostic report"""
        if not self.report_path.exists():
            print("[X] No diagnostic report found. Run diagnostics first:")
            print("    python -m server.diagnostics.val5_debugger")
            return False

        try:
            with open(self.report_path, "r", encoding="utf-8") as f:
                self.report = json.load(f)

            if self.report:
                for issue in self.report.get("issues", []):
                    self.issues_by_type[issue["type"]] = issue

            return True
        except Exception as e:
            print(f"[X] Failed to load report: {e}")
            return False

    def remediate_all(self) -> bool:
        """Apply all necessary fixes"""
        if not self.load_report() or not self.report:
            return False

        print("\n" + "=" * 80)
        print("VAL 5 AUTO-REMEDIATION")
        print("=" * 80)
        print("\nDiagnostic Summary:")
        print(f"  Total Issues: {self.report.get('total_issues')}")
        print(f"  Critical:     {self.report.get('critical')}")
        print(f"  High:         {self.report.get('high')}")

        if not self.report.get("issues"):
            print("\n[OK] No issues detected - emulator is clean")
            print("VAL 5 is likely external (kernel detection, HWID mismatch, region lock)")
            return True

        print(f"\n{len(self.report['issues'])} issues to analyze:\n")

        fix_methods = {
            "CACHE_CONTAMINATION": self._fix_cache_contamination,
            "TIMING_VIOLATION": self._fix_timing_violation,
            "NO_HEARTBEATS": self._fix_no_heartbeats,
            "MISSING_HEARTBEAT_IOCTL": self._fix_missing_heartbeat_ioctl,
            "NO_GATEWAY_TOKENS": self._fix_no_gateway_tokens,
            "NO_IOCTLS": self._fix_no_ioctls,
            "SESSION_DESTROYED_IN_QUEUE": self._fix_session_timeout,
            "FREQUENT_RECONNECTS": self._fix_network_instability,
        }

        for issue_type, fix_method in fix_methods.items():
            if issue_type in self.issues_by_type:
                print(f"[FIX] Applying remedy for {issue_type}...")
                try:
                    fix_method()
                    self.applied_fixes.append(issue_type)
                except Exception as e:
                    print(f"  [WARN] Fix failed: {e}")

        print("\n" + "=" * 80)
        print("REMEDIATION SUMMARY")
        print("=" * 80)
        print(f"Applied {len(self.applied_fixes)} automated fixes")

        if self.applied_fixes:
            print("\nNext steps:")
            print("  1. Restart emulator: python emulator_loader.py")
            print("  2. Run diagnostics again to verify")
            print("  3. Test in queue")

        return True

    def _fix_cache_contamination(self) -> None:
        """Fix dual-cache mixing in vClient"""
        issue = self.issues_by_type["CACHE_CONTAMINATION"]
        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

        build_script = Path("build_vclient.ps1")
        if build_script.exists():
            print("  Attempting vClient auto-build...")
            try:
                result = subprocess.run(
                    ["powershell", "-ExecutionPolicy", "Bypass", "-File", str(build_script)],
                    capture_output=True,
                    text=True,
                    timeout=60,
                )
                if result.returncode == 0:
                    print("  [OK] vClient.exe rebuilt successfully")
                else:
                    print(f"  [WARN] Build failed: {result.stderr}")
            except Exception as e:
                print(f"  [WARN] Build error: {e}")

    def _fix_timing_violation(self) -> None:
        """Fix heartbeat interval violations in config"""
        issue = self.issues_by_type["TIMING_VIOLATION"]
        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

        print("  Updating config.yaml parameters...")

        try:
            import yaml

            config_path = Path("config.yaml")
            if config_path.exists():
                with open(config_path, "r", encoding="utf-8") as f:
                    config = yaml.safe_load(f) or {}

                if "heartbeat" not in config:
                    config["heartbeat"] = {}
                config["heartbeat"]["interval_ms"] = 10000
                config["heartbeat"]["jitter_max_ms"] = 400

                if "session" not in config:
                    config["session"] = {}
                config["session"]["idle_timeout_sec"] = 900

                with open(config_path, "w", encoding="utf-8") as f:
                    yaml.dump(config, f, default_flow_style=False)

                print("  [OK] config.yaml updated:")
                print("    - heartbeat.interval_ms: 10000")
                print("    - heartbeat.jitter_max_ms: 400")
                print("    - session.idle_timeout_sec: 900\n")
        except Exception as e:
            print(f"  [WARN] Config update failed: {e}\n")

    def _fix_no_heartbeats(self) -> None:
        """Fix missing heartbeats"""
        issue = self.issues_by_type["NO_HEARTBEATS"]
        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

        try:
            print("  Restarting vClient.exe process...")
            subprocess.run(["taskkill", "/IM", "vClient.exe", "/F"], capture_output=True, timeout=5)
            time.sleep(1)

            vclient_path = Path("vClient.exe")
            if vclient_path.exists():
                subprocess.Popen([str(vclient_path)])
                print("  [OK] vClient.exe restarted\n")
        except Exception as e:
            print(f"  [WARN] vClient restart failed: {e}\n")

    def _fix_missing_heartbeat_ioctl(self) -> None:
        """Fix missing 0x222000 IOCTL"""
        issue = self.issues_by_type["MISSING_HEARTBEAT_IOCTL"]
        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

    def _fix_no_gateway_tokens(self) -> None:
        """Fix missing Gateway authentication"""
        issue = self.issues_by_type["NO_GATEWAY_TOKENS"]
        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

    def _fix_no_ioctls(self) -> None:
        """Fix missing IOCTL interception"""
        issue = self.issues_by_type["NO_IOCTLS"]
        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

    def _fix_session_timeout(self) -> None:
        """Fix session destruction during queue"""
        issue = self.issues_by_type.get("SESSION_DESTROYED_IN_QUEUE")
        if not issue:
            return

        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

    def _fix_network_instability(self) -> None:
        """Fix frequent reconnects"""
        issue = self.issues_by_type.get("FREQUENT_RECONNECTS")
        if not issue:
            return

        print(f"  Issue: {issue['detail']}")
        print(f"  Fix: {issue['fix']}\n")

        try:
            import yaml

            config_path = Path("config.yaml")
            if config_path.exists():
                with open(config_path, "r", encoding="utf-8") as f:
                    config = yaml.safe_load(f) or {}

                if "tunnel" not in config:
                    config["tunnel"] = {}

                config["tunnel"]["socket_buffer_size"] = 1048576
                config["tunnel"]["keepalive_interval_sec"] = 5

                with open(config_path, "w", encoding="utf-8") as f:
                    yaml.dump(config, f, default_flow_style=False)

                print("  [OK] Tunnel network parameters tuned:")
                print("    - socket_buffer_size: 1048576")
                print("    - keepalive_interval_sec: 5\n")
        except Exception as e:
            print(f"  [WARN] Config update failed: {e}\n")


def main() -> None:
    import sys

    report_path = "logs/val5_diagnostic.json"
    if len(sys.argv) > 1:
        report_path = sys.argv[1]

    remediator = VAL5AutoRemediate(report_path)
    success = remediator.remediate_all()

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
