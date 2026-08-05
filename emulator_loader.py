"""
VGC Emulator Loader - Modern UI
Interactive loader that waits for game stages
"""
import tkinter as tk
from tkinter import ttk
import threading
import time
import psutil
import os
import sys
import subprocess
import socket
import ssl
import struct
import json
import re

SERVER_IP = "192.168.1.136"   # <--- Cambia esto por la IP real de tu VPS / Servidor
SERVER_PORT = 51820

QUEUE_WINDOW_SEC = 240      # Ventana de 4 minutos segura para buscar partida (Queue)
REAUTH_COOLDOWN_SEC = 60    # Cooldown de 60s para recargar / refrescar auth
STATE_FILE = os.path.join(os.path.dirname(__file__), "data", "loader_state.json")

try:
    import yaml
except ImportError:
    yaml = None

class EmulatorLoader:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("VGC Emulator - Esperanza")
        self.root.geometry("600x400")
        self.root.configure(bg='#1a1a1a')
        self.root.resizable(False, False)
        
        # State variables
        self.progress = 0
        self.current_stage = 0
        self.stages = [
            {"name": "Verifying server connection", "progress": 10, "done": False},
            {"name": "Killing stale processes", "progress": 20, "done": False},
            {"name": "Launching Riot client & tunnel", "progress": 30, "done": False},
            {"name": "Waiting for VALORANT main menu", "progress": 60, "done": False},
            {"name": "Bypassing VGC check", "progress": 75, "done": False},
            {"name": "Establishing heartbeats", "progress": 90, "done": False},
            {"name": "Sending auth request", "progress": 100, "done": False},
                ]
        
        self.server_running = False
        self.vclient_running = False
        self.game_detected = False
        self.ready_to_inject = False
        self._vclient_session_id = None
        self._server_ip = None
        self._server_port = None
        
        # Create UI
        self.create_ui()
        
        # Center window
        self.center_window()
        
    def save_auth_state(self):
        """Save auth timestamp to disk for persistence across restarts"""
        try:
            os.makedirs(os.path.dirname(STATE_FILE), exist_ok=True)
            data = {
                "last_auth_timestamp": time.time(),
                "queue_window_sec": QUEUE_WINDOW_SEC,
                "reauth_cooldown_sec": REAUTH_COOLDOWN_SEC
            }
            with open(STATE_FILE, "w", encoding="utf-8") as f:
                json.dump(data, f, indent=2)
        except Exception as e:
            print(f"Failed to save auth state: {e}")

    def get_last_auth_timestamp(self):
        """Load last auth timestamp from disk"""
        try:
            if os.path.exists(STATE_FILE):
                with open(STATE_FILE, "r", encoding="utf-8") as f:
                    data = json.load(f)
                    return float(data.get("last_auth_timestamp", 0))
        except Exception:
            pass
        return 0.0

    def get_auth_cooldown_remaining(self):
        """Seconds remaining in recharge cooldown before another refresh is allowed"""
        last_ts = self.get_last_auth_timestamp()
        if last_ts == 0:
            return 0
        elapsed = time.time() - last_ts
        return max(0, int(REAUTH_COOLDOWN_SEC - elapsed))

    def get_queue_window_remaining(self):
        """Seconds remaining in current safe queueing window"""
        last_ts = self.get_last_auth_timestamp()
        if last_ts == 0:
            return 0
        elapsed = time.time() - last_ts
        return max(0, int(QUEUE_WINDOW_SEC - elapsed))

    def center_window(self):
        self.root.update_idletasks()
        width = self.root.winfo_width()
        height = self.root.winfo_height()
        x = (self.root.winfo_screenwidth() // 2) - (width // 2)
        y = (self.root.winfo_screenheight() // 2) - (height // 2)
        self.root.geometry(f'{width}x{height}+{x}+{y}')
    
    def create_ui(self):
        # Title
        title_label = tk.Label(
            self.root,
            text="VGC EMULATOR",
            font=("Consolas", 24, "bold"),
            bg='#1a1a1a',
            fg='#9d4edd'
        )
        title_label.pack(pady=20)
        
        # Subtitle
        subtitle = tk.Label(
            self.root,
            text="Esperanza v1.0",
            font=("Consolas", 10),
            bg='#1a1a1a',
            fg='#666666'
        )
        subtitle.pack()
        
        # Progress frame
        self.progress_frame = tk.Frame(self.root, bg='#1a1a1a')
        self.progress_frame.pack(pady=30)
        
        # Circular progress (simulated with label)
        self.progress_label = tk.Label(
            self.progress_frame,
            text="0%",
            font=("Consolas", 36, "bold"),
            bg='#1a1a1a',
            fg='#9d4edd',
            width=6
        )
        self.progress_label.pack()
        
        # Progress bar
        style = ttk.Style()
        style.theme_use('default')
        style.configure(
            "purple.Horizontal.TProgressbar",
            troughcolor='#2a2a2a',
            bordercolor='#9d4edd',
            background='#9d4edd',
            lightcolor='#c77dff',
            darkcolor='#7b2cbf'
        )
        
        self.progress_bar = ttk.Progressbar(
            self.progress_frame,
            length=400,
            mode='determinate',
            style="purple.Horizontal.TProgressbar"
        )
        self.progress_bar.pack(pady=10)
        
        # Stages list
        self.stages_frame = tk.Frame(self.root, bg='#1a1a1a')
        self.stages_frame.pack(pady=20)
        
        self.stage_labels = []
        self.stage_indicators = []
        
        for stage in self.stages:
            stage_row = tk.Frame(self.stages_frame, bg='#1a1a1a')
            stage_row.pack(anchor='w', pady=3)
            
            # Indicator (bullet point)
            indicator = tk.Label(
                stage_row,
                text="●",
                font=("Consolas", 14),
                bg='#1a1a1a',
                fg='#444444',
                width=2
            )
            indicator.pack(side='left')
            self.stage_indicators.append(indicator)
            
            # Stage name
            label = tk.Label(
                stage_row,
                text=stage["name"],
                font=("Consolas", 11),
                bg='#1a1a1a',
                fg='#666666',
                anchor='w'
            )
            label.pack(side='left')
            self.stage_labels.append(label)
        
        # Exit button (bottom right)
        exit_button = tk.Button(
            self.root,
            text="Exit",
            font=("Consolas", 10),
            bg='#1a1a1a',
            fg='#666666',
            activebackground='#1a1a1a',
            activeforeground='#999999',
            border=0,
            cursor='hand2',
            command=self.exit_app
        )
        exit_button.pack(side='bottom', pady=10)
        
        # Status label (bottom)
        self.status_label = tk.Label(
            self.root,
            text="Starting emulator...",
            font=("Consolas", 9),
            bg='#1a1a1a',
            fg='#9d4edd'
        )
        self.status_label.pack(side='bottom', pady=5)
    
    def update_progress(self, progress, stage_index=None):
        """Update progress bar and percentage (Thread-safe)"""
        def _update():
            self.progress = progress
            self.progress_bar['value'] = progress
            self.progress_label.config(text=f"{int(progress)}%")
            
            # Update stage indicators
            if stage_index is not None and stage_index < len(self.stages):
                self.current_stage = stage_index
                
                for i, (indicator, label) in enumerate(zip(self.stage_indicators, self.stage_labels)):
                    if i < stage_index:
                        # Completed stages
                        indicator.config(fg='#00ff00')
                        label.config(fg='#00ff00')
                    elif i == stage_index:
                        # Current stage
                        indicator.config(fg='#9d4edd')
                        label.config(fg='#ffffff')
                    else:
                        # Pending stages
                        indicator.config(fg='#444444')
                        label.config(fg='#666666')
        
        self.root.after(0, _update)
    
    def update_status(self, message):
        """Update status message (Thread-safe)"""
        def _update():
            if hasattr(self, 'status_label') and self.status_label.winfo_exists():
                self.status_label.config(text=message)
        self.root.after(0, _update)
    
    def update_stage_status(self, stage_index, message):
        """Update a specific stage's display text (Thread-safe)"""
        def _update():
            if stage_index < len(self.stage_labels):
                self.stage_labels[stage_index].config(text=message)
        self.root.after(0, _update)

    # ──────────────────────────────────────────────────────────
    #  Protocol & Verification Helpers
    # ──────────────────────────────────────────────────────────

    def _create_tls_context(self):
        """Create a reusable TLS context for server connections"""
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        return ctx

    def _open_server_socket(self, host, port, timeout=5):
        """Open a TLS socket to the server. Returns socket or None."""
        try:
            ctx = self._create_tls_context()
            raw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            raw.settimeout(timeout)
            raw.connect((host, port))
            sock = ctx.wrap_socket(raw, server_hostname=host)
            return sock
        except Exception as e:
            print(f"[VGC-EMU] Socket open failed {host}:{port}: {e}")
            return None

    def _protocol_ping(self, host, port, timeout=3):
        """Send protocol PING (type 7) and verify PONG (type 8)."""
        sock = None
        try:
            sock = self._open_server_socket(host, port, timeout)
            if not sock:
                return False
            sock.sendall(struct.pack("!II", 7, 0))
            sock.settimeout(timeout)
            header = sock.recv(8)
            if len(header) == 8:
                msg_type, _ = struct.unpack("!II", header)
                return msg_type == 8
            return False
        except Exception as e:
            print(f"[VGC-EMU] Protocol PING failed: {e}")
            return False
        finally:
            if sock:
                try:
                    sock.close()
                except Exception:
                    pass

    def _protocol_send_ioctl(self, host, port, ioctl_code=0x22C0EC, data=b"", timeout=5):
        """Open a throwaway authenticated connection, send IOCTL, return response bytes.
        
        Sends a minimal SESSION_AUTH first (requires auth_key from config),
        then sends the IOCTL and reads the IOCTL_RESP.  Returns response bytes or None.
        """
        sock = None
        try:
            sock = self._open_server_socket(host, port, timeout)
            if not sock:
                return None

            # Read auth_key from config
            config_path = os.path.join(os.path.dirname(__file__), "config.yaml")
            auth_key = ""
            if os.path.exists(config_path) and yaml:
                with open(config_path, 'r') as f:
                    cfg = yaml.safe_load(f) or {}
                    auth_key = cfg.get('tunnel', {}).get('auth_key', '')
            if not auth_key:
                print("[VGC-EMU] No auth_key for IOCTL probe")
                return None

            # Helper: length-prefixed bytes
            def _lp(b: bytes) -> bytes:
                return struct.pack("!I", len(b)) + b

            # Minimal SESSION_AUTH payload
            body = _lp(auth_key.encode("utf-8"))
            body += _lp(b"probe")                                             # gateway_machine_id
            body += _lp(b"probe_jwt_placeholder")                              # jwt  (non-empty)
            body += _lp(b"00000000-0000-0000-0000-000000000000")               # puuid
            body += struct.pack("!I", 0)                                       # valorant_pid
            body += struct.pack("!Q", int(time.time() * 1000))                 # client_ts_ms

            # Send SESSION_AUTH (type 14)
            sock.sendall(struct.pack("!II", 14, len(body)) + body)

            # Read response header (full 8 bytes)
            hdr = self._recv_exact(sock, 8, timeout)
            if not hdr:
                return None
            msg_type, plen = struct.unpack("!II", hdr)
            payload = self._recv_exact(sock, plen, timeout) if plen else b""

            if msg_type != 15:  # not SESSION_AUTH_OK
                err = payload.decode('utf-8', errors='replace') if msg_type == 9 else f"type={msg_type}"
                print(f"[VGC-EMU] IOCTL probe auth failed: {err}")
                return None

            # Extract probe session id (for logging)
            if payload and len(payload) >= 4:
                slen = struct.unpack_from("!I", payload, 0)[0]
                probe_sid = payload[4:4+slen].decode("utf-8", errors="replace")
                print(f"[VGC-EMU] Probe session: {probe_sid[:8]}")

            # Now send IOCTL (type 4)
            ioctl_body = struct.pack("!I", ioctl_code) + struct.pack("!I", len(data)) + data
            sock.sendall(struct.pack("!II", 4, len(ioctl_body)) + ioctl_body)

            # Read IOCTL_RESP (type 5)
            hdr2 = self._recv_exact(sock, 8, timeout)
            if not hdr2:
                return None
            mt2, pl2 = struct.unpack("!II", hdr2)
            resp = self._recv_exact(sock, pl2, timeout) if pl2 else b""

            if mt2 == 5 and resp and len(resp) >= 4:  # IOCTL_RESP
                dlen = struct.unpack_from("!I", resp, 0)[0]
                return resp[4:4+dlen]
            return None

        except Exception as e:
            print(f"[VGC-EMU] IOCTL probe failed: {e}")
            return None
        finally:
            if sock:
                try:
                    sock.close()
                except Exception:
                    pass

    def _recv_exact(self, sock, n, timeout=5):
        """Read exactly n bytes from socket.  Returns bytes or None."""
        sock.settimeout(timeout)
        buf = b""
        while len(buf) < n:
            chunk = sock.recv(n - len(buf))
            if not chunk:
                return None
            buf += chunk
        return buf

    def _check_vclient_alive(self):
        """Check if vClient.exe process is running"""
        try:
            for proc in psutil.process_iter(['name']):
                if 'vclient' in proc.info['name'].lower():
                    return True
        except Exception:
            pass
        return False

    def _parse_vclient_log(self):
        """Parse vClient.log for session ID, IOCTL activity, and tunnel status.
        
        Returns: (session_id: str|None, ioctl_active: bool, tunnel_active: bool)
        """
        log_path = os.path.join(os.path.dirname(__file__), "vClient.log")
        session_id = None
        ioctl_active = False
        tunnel_active = False

        if not os.path.exists(log_path):
            return session_id, ioctl_active, tunnel_active

        try:
            with open(log_path, 'r', errors='replace') as f:
                lines = f.readlines()

            scan_lines = lines[-100:] if len(lines) > 100 else lines

            for line in scan_lines:
                lu = line.upper()

                # Session ID extraction (UUID-4 format)
                if not session_id:
                    m = re.search(
                        r'(?:SESSION_AUTH_OK|SESSION|session)[=:\s]+([a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12})',
                        line, re.IGNORECASE
                    )
                    if m:
                        session_id = m.group(1)

                # IOCTL activity markers
                if not ioctl_active:
                    ioctl_markers = [
                        '0X22C0EC', 'DRIVER_STATUS', 'IOCTL_RESP', 'IOCTL-KEEPALIVE',
                        '0X222000', 'HEARTBEAT', 'HB ACK', 'VGK PING ACK',
                        'PIPE][HB]', 'PIPE][COMPAT]',
                    ]
                    ok_markers = ['OK', 'RESP', 'ACK', 'WRITTEN', 'RECV', 'BYTES']
                    if any(mk in lu for mk in ioctl_markers):
                        if any(ok in lu for ok in ok_markers):
                            ioctl_active = True

                # Tunnel-level activity markers
                if not tunnel_active:
                    tunnel_markers = [
                        'TUNNEL ACTIVE', 'CONNECTED TO SERVER', 'TLS CONNECTED',
                        'SESSION_AUTH_OK', 'PONG RECV', 'HELLO_OK',
                        'TUNNEL ESTABLISHED', 'SERVER CONNECTED',
                    ]
                    if any(mk in lu for mk in tunnel_markers):
                        tunnel_active = True

                if session_id and ioctl_active and tunnel_active:
                    break

        except Exception as e:
            print(f"[VGC-EMU] Error parsing vClient.log: {e}")

        return session_id, ioctl_active, tunnel_active

    def _bypass_vgc_check(self):
        """Stage 4: Multi-signal VGC tunnel verification with retries.

        Checks four independent signals with escalating fallbacks:
          1. vClient process alive
          2. vClient.log shows IOCTL / tunnel activity
          3. Direct IOCTL 0x22C0EC probe through protocol
          4. Server PING + vClient alive (last resort)
        """
        # ── Signal 1: vClient must be alive ──
        if not self._check_vclient_alive():
            self.update_status("⚠️ vClient not running — restarting...")
            if not self.start_vclient():
                self.update_status("❌ Failed to restart vClient")
                return False
            time.sleep(3)
            if not self._check_vclient_alive():
                self.update_status("❌ vClient failed to start")
                return False

        server_ip, server_port = self.get_server_config()

        # ── Signal 2: Wait for vClient.log to show activity ──
        max_wait_sec = 45
        verified_ioctl = False
        verified_tunnel = False

        for attempt in range(max_wait_sec):
            session_id, ioctl_ok, tunnel_ok = self._parse_vclient_log()

            if session_id and not self._vclient_session_id:
                self._vclient_session_id = session_id
                print(f"[VGC-EMU] Session detected from log: {session_id[:8]}...")

            if ioctl_ok:
                verified_ioctl = True
            if tunnel_ok:
                verified_tunnel = True

            # Best case: IOCTL confirmed in log
            if verified_ioctl:
                sid_str = session_id[:8] if session_id else '???'
                self.update_status(f"✓ IOCTL tunnel verified (session {sid_str})")
                return True

            # Good case: tunnel up for 5s + server responds
            if verified_tunnel and attempt >= 5:
                if self._protocol_ping(server_ip, server_port):
                    self.update_status("✓ Tunnel + server alive — bypass confirmed")
                    return True

            # Check vClient still alive periodically
            if attempt > 0 and attempt % 10 == 0:
                if not self._check_vclient_alive():
                    self.update_status("❌ vClient died during verification")
                    return False

            # Progress feedback
            progress = 60 + min(14, (attempt / max_wait_sec) * 14)
            self.update_progress(progress, 4)
            if verified_tunnel:
                self.update_status(f"Tunnel active, waiting for IOCTL... ({attempt+1}s)")
            else:
                self.update_status(f"Waiting for VGC tunnel... ({attempt+1}/{max_wait_sec}s)")
            time.sleep(1)

        # ── Signal 3: Direct IOCTL 0x22C0EC probe ──
        self.update_status("Attempting direct IOCTL 0x22C0EC probe...")
        ioctl_resp = self._protocol_send_ioctl(server_ip, server_port, ioctl_code=0x22C0EC)
        if ioctl_resp and len(ioctl_resp) > 4:
            print(f"[VGC-EMU] IOCTL 0x22C0EC probe got {len(ioctl_resp)} bytes")
            self.update_status("✓ IOCTL 0x22C0EC driver status verified via probe")
            return True

        # ── Signal 4: Fallback — vClient alive + PING ──
        if self._check_vclient_alive():
            if self._protocol_ping(server_ip, server_port):
                self.update_status("⚠️ IOCTL unverified but server+vClient alive — proceeding")
                return True
            self.update_status("❌ Server unreachable — bypass failed")
            return False

        self.update_status("❌ VGC bypass verification failed")
        return False

    def start_emulator(self):
        """Start the emulation process - called automatically on init"""
        threading.Thread(target=self.emulator_sequence, daemon=True).start()
    
    def emulator_sequence(self):
        """Main emulator sequence"""
        try:
            # Stage 0: Verify server connection (0% -> 10%)
            self.update_status("Verifying connection to emulator server...")
            self.update_progress(0, 0)
            
            # Load server config
            server_ip, server_port = self.get_server_config()
            
            # Try connection with retries
            max_retries = 3
            for attempt in range(1, max_retries + 1):
                self.update_status(f"Connecting to {server_ip}:{server_port} (attempt {attempt}/{max_retries})...")
                
                if self.verify_server_connection(server_ip, server_port):
                    self.update_status(f"✓ Server connected: {server_ip}:{server_port}")
                    time.sleep(0.5)
                    self.update_progress(10, 0)
                    self.stages[0]["done"] = True
                    break
                
                if attempt < max_retries:
                    self.update_status(f"Retry {attempt}/{max_retries-1}...")
                    time.sleep(1)
                else:
                    # Failed all retries
                    self.update_status(f"❌ Cannot connect to server {server_ip}:{server_port}")
                    self.show_server_error(server_ip, server_port)
                    return
            
            # Stage 1: Clean environment and prepare VGC (10% -> 20%)
            self.update_status("Preparing VGC environment...")
            self.update_progress(10, 1)
            self.prepare_vgc_environment()
            time.sleep(0.5)
            self.update_progress(20, 1)
            self.stages[1]["done"] = True
            
            # Stage 2: Launch vClient & Riot Client (20% -> 30%)
            self.update_status("Starting vClient tunnel & Riot Client...")
            self.update_progress(20, 2)
            if not self.start_vclient():
                self.update_status("❌ Failed to start vClient")
                return
            time.sleep(1)
            if not self.start_riot_client():
                self.update_status("⚠️ Riot Client launch skipped or already running")
            time.sleep(2)
            self.update_progress(30, 2)
            self.stages[2]["done"] = True
            
            # Stage 3: Wait for Valorant main menu to fully load (30% -> 60%)
            self.update_status("⏳ Waiting for VALORANT main menu to load...")
            self.update_progress(30, 3)
            if not self.wait_for_game():
                self.update_status("❌ Timeout waiting for game")
                return
            self.update_progress(60, 3)
            self.stages[3]["done"] = True
            
                                    
            # Stage 4: Bypass VGC check AFTER game is loaded (60% -> 75%)
            self.update_status("Verifying VGC tunnel active...")
            self.update_progress(60, 4)

            if not self._bypass_vgc_check():
                self.update_status("❌ VGC bypass failed — cannot queue safely")
                return

            time.sleep(0.5)
            self.update_progress(75, 4)
            self.stages[4]["done"] = True
            
            # Stage 5: Establish heartbeats (75% -> 90%)
            self.update_status("Establishing heartbeats with server...")
            self.update_progress(75, 5)
            if not self.establish_heartbeats():
                self.update_status("❌ Heartbeat connection failed")
                return
            time.sleep(2)
            self.update_progress(90, 5)
            self.stages[5]["done"] = True
            
            # Stage 6: Send auth request (90% -> 100%) - FINAL STEP AT LOBBY
            self.update_status("Sending auth request (Final Step)...")
            self.update_progress(90, 6)
            time.sleep(1)
            
            if not self.send_auth_request():
                self.update_status("❌ Auth request failed")
                return
            
            time.sleep(1)
            self.update_progress(100, 6)
            self.stages[6]["done"] = True
            
            # Success!
            self.show_ready_screen()
            
        except Exception as e:
            self.update_status(f"❌ Error: {str(e)}")
            print(f"Error in emulator sequence: {e}")

    def kill_stale_processes(self):
        """Kill old VGC/vClient/game processes"""
        processes_to_kill = ['vgc.exe', 'vgtray.exe', 'vclient.exe', 'vgc_client.exe']
        killed = []
        for proc in psutil.process_iter(['name']):
            try:
                name = proc.info['name'].lower()
                if any(k in name for k in processes_to_kill):
                    proc.kill()
                    killed.append(proc.info['name'])
            except:
                pass
        print(f"[VGC-EMU] Killed stale processes: {killed if killed else 'none'}")
    
    def prepare_vgc_environment(self):
        """Ensure clean environment for vClient without corrupting Windows services"""
        self.update_stage_status(1, "⚙️ Preparing VGC environment...")
        self.kill_stale_processes()
        try:
            # 1. Stop real VGC service if running so vClient can host the named pipes
            print("[VGC-EMU] Stopping VGC service if active...")
            subprocess.run('sc stop vgc', shell=True, capture_output=True, timeout=3)
            time.sleep(0.5)
            
            # 2. Ensure vgc service exists and points to the legitimate binary with demand start
            vgc_bin = r"C:\Program Files\Riot Vanguard\vgc.exe"
            query_res = subprocess.run('sc query vgc', shell=True, capture_output=True, text=True, timeout=3)
            if "1060" in query_res.stdout or "1060" in query_res.stderr or query_res.returncode != 0:
                print("[VGC-EMU] Service vgc not found, registering legitimate Vanguard service...")
                subprocess.run(f'sc create vgc binPath= "{vgc_bin}" start= demand DisplayName= "Vanguard Service"',
                               shell=True, capture_output=True, timeout=3)
            else:
                if os.path.exists(vgc_bin):
                    subprocess.run(f'sc config vgc binPath= "{vgc_bin}" start= demand DisplayName= "Vanguard Service"',
                                   shell=True, capture_output=True, timeout=3)
                else:
                    subprocess.run('sc config vgc start= demand', shell=True, capture_output=True, timeout=3)
            print("[VGC-EMU] VGC environment prepared successfully")
            return True
        except subprocess.TimeoutExpired:
            print("[VGC-EMU] Warning: VGC service commands timed out (continuing anyway)")
            return True
        except Exception as e:
            print(f"[VGC-EMU] Warning: Service preparation non-fatal error: {e}")
            return True
    
    def restore_vanguard_service(self):
        """Restore VGC service to original state (optional cleanup)"""
        try:
            subprocess.run(['sc', 'config', 'vgc', 'start=', 'auto'], capture_output=True, timeout=5)
            print("[VGC-EMU] VGC service restored to auto-start")
        except Exception as e:
            print(f"[VGC-EMU] Warning: Could not restore VGC service: {e}")
    
    def get_server_config(self):
        """Get server IP and port from config.yaml or vclient_config.h or SERVER_IP"""
        missing_data = []
        
        # Check config.yaml first (Option A & Tunnel config)
        config_path = os.path.join(os.path.dirname(__file__), "config.yaml")
        if os.path.exists(config_path) and yaml:
            try:
                with open(config_path, 'r') as f:
                    config = yaml.safe_load(f) or {}
                    
                    # Option A: Check client section first
                    client_cfg = config.get('client', {})
                    if 'server_ip' in client_cfg:
                        host = client_cfg.get('server_ip', SERVER_IP)
                        port = int(client_cfg.get('server_port', SERVER_PORT))
                        if host == '127.0.0.1':
                            missing_data.append("⚠️ Using localhost (127.0.0.1) - make sure server is on same PC")
                        return host, port
            except Exception as e:
                missing_data.append(f"Error reading config.yaml client section: {e}")
        
        # Try to read from vclient_config.h (build-time config)
        config_h_path = os.path.join(os.path.dirname(__file__), "server", "vclient_config.h")
        if os.path.exists(config_h_path):
            try:
                with open(config_h_path, 'r') as f:
                    content = f.read()
                    import re
                    host_match = re.search(r'#define VPS_HOST_AUTO "([^"]+)"', content)
                    port_match = re.search(r'#define VPS_PORT_AUTO (\d+)', content)
                    if host_match and port_match:
                        host = host_match.group(1)
                        port = int(port_match.group(1))
                        
                        if host == '127.0.0.1':
                            missing_data.append("⚠️ Using localhost (127.0.0.1) - gaming PC = server PC?")
                        
                        if missing_data:
                            self.show_config_warning(host, port, missing_data)
                        
                        return host, port
            except Exception as e:
                missing_data.append(f"Error reading vclient_config.h: {e}")
        
        # Fallback: Try tunnel section in config.yaml
        if os.path.exists(config_path) and yaml:
            try:
                with open(config_path, 'r') as f:
                    config = yaml.safe_load(f) or {}
                    host = config.get('tunnel', {}).get('host', SERVER_IP)
                    port = int(config.get('tunnel', {}).get('port', SERVER_PORT))
                    if host == '0.0.0.0':
                        host = SERVER_IP
                        missing_data.append(f"⚠️ config.yaml tunnel has 0.0.0.0, using server IP: {SERVER_IP}")
                    
                    if missing_data:
                        self.show_config_warning(host, port, missing_data)
                    
                    return host, port
            except Exception as e:
                missing_data.append(f"Error reading config.yaml tunnel section: {e}")
        
        # Option B Default: Use SERVER_IP and SERVER_PORT
        host, port = SERVER_IP, SERVER_PORT
        missing_data.append(f"⚠️ Using default SERVER_IP/SERVER_PORT: {host}:{port}")
        self.show_config_warning(host, port, missing_data)
        
        return host, port
    
    def show_config_warning(self, host, port, warnings):
        """Show warning dialog with config issues (non-blocking)"""
        print("\n" + "="*60)
        print("⚠️  CONFIG WARNINGS")
        print("="*60)
        for warning in warnings:
            print(f"  {warning}")
        print(f"\nUsing: {host}:{port}")
        print("="*60 + "\n")
    
    def verify_server_connection(self, host, port, timeout=3):
        """Verify connection to emulator server with shorter timeout"""
        try:
            # Try to connect with TLS
            ctx = ssl.create_default_context()
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE
            
            raw_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            raw_sock.settimeout(timeout)
            
            # Try to connect
            raw_sock.connect((host, port))
            
            # Wrap with TLS
            sock = ctx.wrap_socket(raw_sock, server_hostname=host)
            
            # Send PING message (MsgType.PING = 7)
            ping_msg = struct.pack("!II", 7, 0)  # msg_type=7, payload_len=0
            sock.sendall(ping_msg)
            
            # Receive PONG (MsgType.PONG = 8)
            sock.settimeout(2)  # Short timeout for response
            header = sock.recv(8)
            if len(header) == 8:
                msg_type, payload_len = struct.unpack("!II", header)
                sock.close()
                return msg_type == 8  # PONG received
            
            sock.close()
            return False
            
        except (ConnectionRefusedError, socket.timeout, OSError):
            return False
        except Exception as e:
            print(f"Server verification error: {e}")
            return False
    
    def show_server_error(self, host, port):
        """Show error screen for server connection failure"""
        # Clear current UI
        for widget in self.root.winfo_children():
            widget.destroy()
        
        # Error icon
        error_label = tk.Label(
            self.root,
            text="✗",
            font=("Consolas", 48, "bold"),
            bg='#1a1a1a',
            fg='#ff4444'
        )
        error_label.pack(pady=30)
        
        # Error message
        error_msg = tk.Label(
            self.root,
            text="SERVER NOT REACHABLE",
            font=("Consolas", 18, "bold"),
            bg='#1a1a1a',
            fg='#ff4444'
        )
        error_msg.pack(pady=10)
        
        # Details
        details_frame = tk.Frame(self.root, bg='#1a1a1a')
        details_frame.pack(pady=20)
        
        details = [
            f"Cannot connect to emulator server:",
            f"",
            f"Host: {host}",
            f"Port: {port}",
            f"",
            f"Please ensure:",
            f"  • Server is running: python -m server.main",
            f"  • Firewall allows port {port}",
            f"  • IP address is correct",
        ]
        
        for detail in details:
            label = tk.Label(
                details_frame,
                text=detail,
                font=("Consolas", 10),
                bg='#1a1a1a',
                fg='#999999' if detail.startswith(' ') else '#ffffff',
                anchor='w'
            )
            label.pack(anchor='w', padx=40)
        
        # Buttons frame
        buttons_frame = tk.Frame(self.root, bg='#1a1a1a')
        buttons_frame.pack(pady=30)
        
        # Retry button
        retry_btn = tk.Button(
            buttons_frame,
            text="RETRY",
            font=("Consolas", 12, "bold"),
            bg='#9d4edd',
            fg='white',
            activebackground='#c77dff',
            border=0,
            padx=40,
            pady=10,
            cursor='hand2',
            command=self.restart_loader
        )
        retry_btn.pack(side='left', padx=10)
        
        # Edit Config button
        config_btn = tk.Button(
            buttons_frame,
            text="EDIT CONFIG",
            font=("Consolas", 12),
            bg='#2a2a2a',
            fg='#999999',
            activebackground='#3a3a3a',
            border=0,
            padx=30,
            pady=10,
            cursor='hand2',
            command=lambda: self.open_config()
        )
        config_btn.pack(side='left', padx=10)
        
        # Exit button
        exit_button = tk.Button(
            self.root,
            text="Exit",
            font=("Consolas", 10),
            bg='#1a1a1a',
            fg='#666666',
            activebackground='#1a1a1a',
            border=0,
            cursor='hand2',
            command=self.exit_app
        )
        exit_button.pack(pady=20)
    
    def open_config(self):
        """Open config.yaml in default editor"""
        config_path = os.path.join(os.path.dirname(__file__), "config.yaml")
        if os.path.exists(config_path):
            os.startfile(config_path)
        else:
            print(f"Config not found: {config_path}")
    
    def start_vclient(self):
        """Start vClient.exe"""
        try:
            vclient_path = os.path.join(os.path.dirname(__file__), "build", "vClient.exe")
            if not os.path.exists(vclient_path):
                vclient_path = os.path.join(os.path.dirname(__file__), "vClient.exe")
            
            if os.path.exists(vclient_path):
                subprocess.Popen([vclient_path], 
                               creationflags=subprocess.CREATE_NO_WINDOW)
                self.vclient_running = True
                return True
            else:
                print(f"vClient not found at {vclient_path}")
                return False
        except Exception as e:
            print(f"Failed to start vClient: {e}")
            return False

    def start_riot_client(self):
        """Find and launch RiotClientServices.exe for Valorant"""
        try:
            rc_path = None
            
            # 1. Try RiotClientInstalls.json (standard Riot install registry)
            installs_json = r"C:\ProgramData\Riot Games\RiotClientInstalls.json"
            if os.path.exists(installs_json):
                try:
                    with open(installs_json, 'r', encoding='utf-8') as f:
                        data = json.load(f)
                        rc_path = data.get("rc_live") or data.get("rc_default")
                except Exception as e:
                    print(f"Error reading RiotClientInstalls.json: {e}")
            
            # 2. Standard fallback paths if json not found or failed
            if not rc_path or not os.path.exists(rc_path):
                common_paths = [
                    r"C:\Riot Games\Riot Client\RiotClientServices.exe",
                    r"D:\Riot Games\Riot Client\RiotClientServices.exe",
                    r"E:\Riot Games\Riot Client\RiotClientServices.exe"
                ]
                for p in common_paths:
                    if os.path.exists(p):
                        rc_path = p
                        break
            
            if rc_path and os.path.exists(rc_path):
                print(f"Launching Riot Client: {rc_path}")
                subprocess.Popen([
                    rc_path,
                    "--launch-product=valorant",
                    "--launch-patchline=live"
                ])
                return True
            else:
                print("RiotClientServices.exe not found on system.")
                return False
        except Exception as e:
            print(f"Failed to start Riot Client: {e}")
            return False
    
    def wait_for_game(self, timeout=300):
        """Wait for VALORANT-Win64-Shipping.exe to start and stabilize in memory"""
        start_time = time.time()
        found = False
        while time.time() - start_time < timeout:
            for proc in psutil.process_iter(['name', 'status']):
                try:
                    name = proc.info['name'].lower()
                    if name == 'valorant-win64-shipping.exe' or name == 'valorant.exe':
                        found = True
                        break
                except:
                    pass
            
            if found:
                self.update_status("Game detected! Waiting 22s for main lobby menu to render...")
                time.sleep(22)
                self.game_detected = True
                return True

            time.sleep(1)
            elapsed = time.time() - start_time
            progress = 30 + min(30, (elapsed / timeout) * 30)
            self.update_progress(progress, 3)
        return False
    
    def establish_heartbeats(self):
        """Verify heartbeat pipeline: vClient alive + server responsive + log activity."""
        try:
            # 1. vClient must be alive
            if not self._check_vclient_alive():
                self.update_status("❌ vClient process not found")
                return False

            # 2. Check log for heartbeat evidence
            _, ioctl_ok, _ = self._parse_vclient_log()
            if ioctl_ok:
                self.update_status("✓ Heartbeat IOCTL verified from log")
                return True

            # 3. Protocol PING to confirm server is alive and responding
            server_ip, server_port = self.get_server_config()
            max_retries = 3
            for attempt in range(1, max_retries + 1):
                self.update_status(f"Heartbeat PING check ({attempt}/{max_retries})...")
                if self._protocol_ping(server_ip, server_port):
                    self.update_status("✓ Server PONG received — heartbeats active")
                    return True
                time.sleep(1)

            # 4. Fallback: vClient alive = assume heartbeats will flow
            if self._check_vclient_alive():
                self.update_status("⚠️ PONG missed but vClient alive — heartbeats assumed")
                return True

            self.update_status("❌ Heartbeat verification failed")
            return False

        except Exception as e:
            self.update_status(f"❌ Heartbeat error: {str(e)}")
            return False
    
    def send_auth_request(self):
        """Verify auth by confirming session exists (protocol PING) + vClient log.
        
        The actual SESSION_AUTH is handled by vClient.exe — the loader only
        confirms the pipeline is alive before marking auth complete.
        """
        try:
            server_ip, server_port = self.get_server_config()

            # 1. Extract session from vClient log (may already be cached)
            session_id = self._vclient_session_id
            if not session_id:
                sid, _, _ = self._parse_vclient_log()
                if sid:
                    session_id = sid
                    self._vclient_session_id = sid

            # 2. Protocol PING — confirms server is alive and accepting
            auth_confirmed = False
            max_retries = 3
            for attempt in range(1, max_retries + 1):
                self.update_status(f"Auth verification PING ({attempt}/{max_retries})...")
                if self._protocol_ping(server_ip, server_port):
                    auth_confirmed = True
                    break
                time.sleep(1)

            if auth_confirmed:
                self.save_auth_state()
                sid_str = session_id[:8] if session_id else 'N/A'
                self.update_status(f"✓ Auth verified (session {sid_str})")
                return True

            # 3. Fallback: if vClient alive, trust auth was done by vClient
            if self._check_vclient_alive():
                self.save_auth_state()
                self.update_status("⚠️ Server PING missed but vClient alive — auth assumed")
                return True

            self.update_status("❌ Auth verification failed")
            return False

        except Exception as e:
            self.update_status(f"⚠️ Auth error: {str(e)}, proceeding anyway")
            self.save_auth_state()
            return True
    
    def show_ready_screen(self):
        """Show 'Ready to inject' screen with exact queue window and recharge cooldown"""
        # Clear current UI
        for widget in self.root.winfo_children():
            widget.destroy()
        
        # Success icon
        success_label = tk.Label(
            self.root,
            text="✓",
            font=("Consolas", 40, "bold"),
            bg='#1a1a1a',
            fg='#00ff00'
        )
        success_label.pack(pady=(15, 5))
        
        # Queue status title
        self.queue_status_label = tk.Label(
            self.root,
            text="CHECKING QUEUE WINDOW...",
            font=("Consolas", 15, "bold"),
            bg='#1a1a1a',
            fg='#00ff00'
        )
        self.queue_status_label.pack(pady=5)

        # Queue timer label (Exact seconds remaining)
        self.queue_timer_label = tk.Label(
            self.root,
            text="--:--",
            font=("Consolas", 22, "bold"),
            bg='#1a1a1a',
            fg='#ffffff'
        )
        self.queue_timer_label.pack(pady=5)
        
        # Buttons frame
        buttons_frame = tk.Frame(self.root, bg='#1a1a1a')
        buttons_frame.pack(pady=20)
        
        # Refresh button (with cooldown protection)
        self.refresh_btn = tk.Button(
            buttons_frame,
            text="REFRESH AUTH",
            font=("Consolas", 11, "bold"),
            bg='#2a2a2a',
            fg='#666666',
            activebackground='#3a3a3a',
            border=0,
            padx=25,
            pady=10,
            cursor='no',
            state='disabled',
            command=self.restart_loader
        )
        self.refresh_btn.pack(side='left', padx=10)
        
        # Inject button
        inject_btn = tk.Button(
            buttons_frame,
            text="INJECT",
            font=("Consolas", 11, "bold"),
            bg='#9d4edd',
            fg='white',
            activebackground='#c77dff',
            border=0,
            padx=35,
            pady=10,
            cursor='hand2',
            command=self.inject_cheat
        )
        inject_btn.pack(side='left', padx=10)
        
        # Exit button
        exit_button = tk.Button(
            self.root,
            text="Exit",
            font=("Consolas", 10),
            bg='#1a1a1a',
            fg='#666666',
            activebackground='#1a1a1a',
            border=0,
            cursor='hand2',
            command=self.exit_app
        )
        exit_button.pack(pady=10)
        
        # Start live queue window & cooldown loop
        self.update_live_queue_and_cooldown()

    def update_live_queue_and_cooldown(self):
        """Thread-safe live loop for queue window & refresh cooldown"""
        if not hasattr(self, 'queue_timer_label') or not self.queue_timer_label.winfo_exists():
            return

        queue_sec = self.get_queue_window_remaining()
        cooldown_sec = self.get_auth_cooldown_remaining()

        # 1. Update Queue Window status
        if queue_sec > 60:
            mins = queue_sec // 60
            secs = queue_sec % 60
            self.queue_status_label.config(text="🟢 MATCHMAKING QUEUE WINDOW OPEN", fg='#00ff00')
            self.queue_timer_label.config(text=f"{mins:02d}:{secs:02d} ({queue_sec}s remaining)", fg='#00ff00')
        elif queue_sec > 0:
            mins = queue_sec // 60
            secs = queue_sec % 60
            self.queue_status_label.config(text="⚠️ QUEUE WINDOW CLOSING SOON", fg='#ffaa00')
            self.queue_timer_label.config(text=f"{mins:02d}:{secs:02d} ({queue_sec}s remaining)", fg='#ffaa00')
        else:
            self.queue_status_label.config(text="❌ QUEUE WINDOW EXPIRED - RE-AUTHENTICATE", fg='#ff4444')
            self.queue_timer_label.config(text="00:00 (0s remaining)", fg='#ff4444')

        # 2. Update Refresh Button Cooldown State
        if cooldown_sec > 0:
            self.refresh_btn.config(
                text=f"RECHARGING ({cooldown_sec}s)",
                state='disabled',
                bg='#2a2a2a',
                fg='#666666',
                cursor='no'
            )
        else:
            self.refresh_btn.config(
                text="REFRESH AUTH",
                state='normal',
                bg='#9d4edd',
                fg='#ffffff',
                cursor='hand2'
            )

        # Schedule next update in 1 second
        self.root.after(1000, self.update_live_queue_and_cooldown)

    def inject_cheat(self):
        """Inject cheat (if available)"""
        self.update_status("Injection complete!")
    
    def restart_loader(self):
        """Restart the loader if recharge cooldown has expired"""
        cooldown = self.get_auth_cooldown_remaining()
        if cooldown > 0:
            self.update_status(f"⚠️ Auth in recharge! Wait {cooldown}s before refreshing.")
            return

        self.root.destroy()
        self.__init__()
        self.run()
    
    def exit_app(self):
        """Exit application and terminate all game, client and tunnel processes"""
        try:
            processes_to_kill = [
                'valorant-win64-shipping.exe', 
                'valorant.exe', 
                'riotclientservices.exe', 
                'riotclientux.exe', 
                'vclient.exe', 
                'vgc.exe'
            ]
            for proc in psutil.process_iter(['name']):
                try:
                    name = proc.info['name'].lower()
                    if any(p in name for p in processes_to_kill):
                        proc.kill()
                except:
                    pass
        except Exception as e:
            print(f"Error terminating processes on exit: {e}")
        
        # Optionally restore VGC service on exit (uncomment if you want Vanguard back)
        # self.restore_vanguard_service()
        
        self.root.quit()
        self.root.destroy()
        sys.exit(0)
    
    def run(self):
        """Start the GUI and auto-start emulator"""
        # Auto-start emulator after GUI is ready
        self.root.after(500, self.start_emulator)
        self.root.mainloop()

if __name__ == "__main__":
    loader = EmulatorLoader()
    loader.run()
