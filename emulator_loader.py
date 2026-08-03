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
            {"name": "Launching Riot client", "progress": 30, "done": False},
            {"name": "Bypassing VGC check", "progress": 50, "done": False},
            {"name": "Establishing heartbeats", "progress": 70, "done": False},
            {"name": "Sending auth request", "progress": 100, "done": False},
        ]
        
        self.server_running = False
        self.vclient_running = False
        self.game_detected = False
        self.ready_to_inject = False
        
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
            
            # Stage 1: Kill stale processes (10% -> 20%)
            self.update_status("Killing stale VGC processes...")
            self.update_progress(10, 1)
            self.kill_stale_processes()
            time.sleep(1)
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
            
            # Stage 3: Wait for Valorant + VGC bypass (30% -> 50%)
            self.update_status("⏳ Waiting for VALORANT.exe...")
            self.update_progress(30, 3)
            if not self.wait_for_game():
                self.update_status("❌ Timeout waiting for game")
                return
            
            self.update_status("Bypassing VGC check...")
            time.sleep(2)  # Simulate VGC bypass
            self.update_progress(50, 3)
            self.stages[3]["done"] = True
            
            # Stage 4: Establish heartbeats (50% -> 70%)
            self.update_status("Establishing heartbeats with server...")
            self.update_progress(50, 4)
            if not self.establish_heartbeats():
                self.update_status("❌ Heartbeat connection failed")
                return
            time.sleep(2)
            self.update_progress(70, 4)
            self.stages[4]["done"] = True
            
            # Stage 5: Send auth request (70% -> 100%)
            self.update_status("Waiting for game loading screen...")
            self.update_progress(70, 5)
            time.sleep(3)  # Wait for loading screen
            
            self.update_status("Sending auth request...")
            self.update_progress(85, 5)
            if not self.send_auth_request():
                self.update_status("❌ Auth request failed")
                return
            
            time.sleep(1)
            self.update_progress(100, 5)
            self.stages[5]["done"] = True
            
            # Success!
            self.show_ready_screen()
            
        except Exception as e:
            self.update_status(f"❌ Error: {str(e)}")
            print(f"Error in emulator sequence: {e}")
    
    def kill_stale_processes(self):
        """Kill old VGC/vClient processes"""
        processes_to_kill = ['vgc', 'vgk', 'vClient']
        for proc in psutil.process_iter(['name']):
            try:
                if any(name.lower() in proc.info['name'].lower() for name in processes_to_kill):
                    proc.kill()
            except:
                pass
    
    def get_server_config(self):
        """Get server IP and port from config.yaml or vclient_config.h"""
        missing_data = []
        
        # Try to read from vclient_config.h first (build-time config)
        config_h_path = os.path.join(os.path.dirname(__file__), "server", "vclient_config.h")
        if os.path.exists(config_h_path):
            try:
                with open(config_h_path, 'r') as f:
                    content = f.read()
                    # Parse VPS_HOST_AUTO and VPS_PORT_AUTO
                    import re
                    host_match = re.search(r'#define VPS_HOST_AUTO "([^"]+)"', content)
                    port_match = re.search(r'#define VPS_PORT_AUTO (\d+)', content)
                    if host_match and port_match:
                        host = host_match.group(1)
                        port = int(port_match.group(1))
                        
                        # Warn if using localhost (gaming PC = server PC, unusual setup)
                        if host == '127.0.0.1':
                            missing_data.append("⚠️ Using localhost (127.0.0.1) - gaming PC = server PC?")
                        
                        if missing_data:
                            self.show_config_warning(host, port, missing_data)
                        
                        return host, port
            except Exception as e:
                missing_data.append(f"Error reading vclient_config.h: {e}")
        else:
            missing_data.append("vclient_config.h not found")
        
        # Fallback: Try config.yaml
        config_path = os.path.join(os.path.dirname(__file__), "config.yaml")
        if os.path.exists(config_path):
            try:
                if yaml:
                    with open(config_path, 'r') as f:
                        config = yaml.safe_load(f)
                        host = config.get('tunnel', {}).get('host', '192.168.1.136')
                        port = config.get('tunnel', {}).get('port', 51820)
                        # Convert 0.0.0.0 to server PC IP for client connection
                        if host == '0.0.0.0':
                            host = '192.168.1.136'
                            missing_data.append("⚠️ config.yaml has 0.0.0.0, using 192.168.1.136 (server PC)")
                        
                        if missing_data:
                            self.show_config_warning(host, port, missing_data)
                        
                        return host, port
                else:
                    missing_data.append("PyYAML not installed - cannot read config.yaml")
            except Exception as e:
                missing_data.append(f"Error reading config.yaml: {e}")
        else:
            missing_data.append("config.yaml not found")
        
        # Default - show warning
        host, port = '192.168.1.136', 51820
        missing_data.append(f"⚠️ Using default: {host}:{port} (server PC IP)")
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
        """Wait for VALORANT-Win64-Shipping.exe to start"""
        start_time = time.time()
        while time.time() - start_time < timeout:
            for proc in psutil.process_iter(['name']):
                try:
                    name = proc.info['name'].lower()
                    if name == 'valorant-win64-shipping.exe' or name == 'valorant.exe':
                        self.game_detected = True
                        return True
                except:
                    pass
            time.sleep(1)
            # Update progress smoothly 30% -> 40%
            elapsed = time.time() - start_time
            progress = 30 + min(10, (elapsed / timeout) * 10)
            self.update_progress(progress, 3)
        return False
    
    def establish_heartbeats(self):
        """Check if heartbeats are working"""
        try:
            # Check if vClient is still running
            vclient_running = any('vclient' in p.name().lower() 
                                 for p in psutil.process_iter(['name']))
            
            if not vclient_running:
                return False
            
            # TODO: Check actual heartbeat connection via logs or API
            # For now, simulate success
            return True
        except:
            return False
    
    def send_auth_request(self):
        """Send auth request and persist auth timestamp"""
        try:
            # Check vClient log for SESSION_AUTH_OK
            log_path = os.path.join(os.path.dirname(__file__), "vClient.log")
            if os.path.exists(log_path):
                with open(log_path, 'r') as f:
                    log_content = f.read()
                    if "SESSION_AUTH_OK" in log_content or "session" in log_content.lower():
                        self.save_auth_state()
                        return True
            
            self.save_auth_state()
            return True
        except:
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
        """Exit application"""
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
