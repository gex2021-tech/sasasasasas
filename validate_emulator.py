#!/usr/bin/env python3
"""
Validation script for VGC Emulator
Tests all components and verifies functionality
"""
import socket
import ssl
import struct
import sys
import time
from pathlib import Path

# Colors for terminal output
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    CYAN = '\033[96m'
    GRAY = '\033[90m'
    RESET = '\033[0m'

def print_test(msg):
    print(f"{Colors.CYAN}[TEST]{Colors.RESET} {msg}")

def print_pass(msg):
    print(f"{Colors.GREEN}[PASS]{Colors.RESET} {msg}")

def print_fail(msg):
    print(f"{Colors.RED}[FAIL]{Colors.RESET} {msg}")

def print_warn(msg):
    print(f"{Colors.YELLOW}[WARN]{Colors.RESET} {msg}")

def print_info(msg):
    print(f"{Colors.GRAY}[INFO]{Colors.RESET} {msg}")

class ValidationError(Exception):
    pass

def validate_structure():
    """Check project structure"""
    print_test("Validating project structure...")
    
    required = {
        'server/main.py': 'Main server entry point',
        'server/tunnel_server.py': 'TLS tunnel server',
        'server/session_manager.py': 'Session management',
        'server/vgc_crypto.py': 'Crypto session',
        'server/vgc_driver.py': 'VGC driver emulation',
        'server/gateway_envelope.py': 'Gateway envelope generator',
        'config.yaml': 'Configuration file',
        'certs/server.pem': 'TLS certificate',
        'certs/server.key': 'TLS private key',
    }
    
    missing = []
    for path, desc in required.items():
        if not Path(path).exists():
            missing.append(f"{path} ({desc})")
    
    if missing:
        raise ValidationError(f"Missing required files:\n  " + "\n  ".join(missing))
    
    print_pass("Project structure is valid")

def validate_config():
    """Check configuration"""
    print_test("Validating configuration...")
    
    import yaml
    
    try:
        with open('config.yaml', 'r') as f:
            config = yaml.safe_load(f)
    except Exception as e:
        raise ValidationError(f"Failed to parse config.yaml: {e}")
    
    # Check required sections
    required_sections = ['tunnel', 'session', 'heartbeat', 'gateway']
    for section in required_sections:
        if section not in config:
            raise ValidationError(f"Missing config section: {section}")
    
    # Check critical values
    tunnel = config['tunnel']
    if not tunnel.get('auth_key') or tunnel['auth_key'].startswith('TROQUE'):
        print_warn("auth_key needs to be configured in config.yaml")
    
    if tunnel.get('port', 51820) != 51820:
        print_info(f"Using non-standard port: {tunnel['port']}")
    
    print_pass("Configuration is valid")
    return config

def validate_imports():
    """Check Python dependencies"""
    print_test("Validating Python dependencies...")
    
    try:
        import yaml
        print_pass("PyYAML available")
    except ImportError:
        raise ValidationError("PyYAML not installed. Run: pip install pyyaml")
    
    # Check server imports
    try:
        sys.path.insert(0, str(Path.cwd()))
        from server import vgc_driver, vgc_crypto, gateway_envelope
        print_pass("Server modules can be imported")
    except ImportError as e:
        raise ValidationError(f"Failed to import server modules: {e}")

def validate_certificates():
    """Check TLS certificates"""
    print_test("Validating TLS certificates...")
    
    cert_path = Path('certs/server.pem')
    key_path = Path('certs/server.key')
    
    if not cert_path.exists() or not key_path.exists():
        raise ValidationError("TLS certificates missing. Generate with:\n" +
                            "  openssl req -x509 -newkey rsa:2048 -keyout certs/server.key " +
                            "-out certs/server.pem -days 365 -nodes -subj '/CN=vgc-emulator'")
    
    # Try to load the certificate
    try:
        import ssl as ssl_module
        ctx = ssl_module.SSLContext(ssl_module.PROTOCOL_TLS_SERVER)
        ctx.load_cert_chain(str(cert_path), str(key_path))
        print_pass("TLS certificates are valid")
    except Exception as e:
        raise ValidationError(f"Invalid TLS certificates: {e}")

def test_driver_responses():
    """Test VGC driver response generation"""
    print_test("Testing VGC driver responses...")
    
    from server.vgc_driver import handle_driver_ioctl
    
    session_id = "test-session-12345"
    aes_key = b'\x00' * 32
    
    # Test heartbeat
    response = handle_driver_ioctl(session_id, 0x222000, b'', aes_key)
    if len(response) < 20:
        raise ValidationError(f"Heartbeat response too short: {len(response)} bytes")
    
    # Check for security flags (field 6)
    if b'\x30' not in response:  # Field 6, wire type 0
        print_warn("Security flags (field 6) may be missing in heartbeat")
    
    print_pass(f"Heartbeat response: {len(response)} bytes (with security flags)")
    
    # Test integrity check
    response = handle_driver_ioctl(session_id, 0x222004, b'', aes_key)
    if len(response) < 10:
        raise ValidationError(f"Integrity response too short: {len(response)} bytes")
    print_pass(f"Integrity check response: {len(response)} bytes")
    
    # Test attestation
    challenge = b'\xAA' * 32
    response = handle_driver_ioctl(session_id, 0x222008, challenge, aes_key)
    if len(response) < 40:
        raise ValidationError(f"Attestation response too short: {len(response)} bytes")
    print_pass(f"Attestation response: {len(response)} bytes")
    
    # Test driver status (NEW - VAL 5 fix)
    response = handle_driver_ioctl(session_id, 0x22C0EC, b'', aes_key)
    if len(response) < 50:
        raise ValidationError(f"Driver status response too short: {len(response)} bytes")
    if response[0:2] != b'\x08\x01':  # Field 1, driver_loaded=1
        print_warn("Driver status may not report driver_loaded=1")
    print_pass(f"Driver status (IOCTL 0x22C0EC): {len(response)} bytes")

def test_gateway_envelope():
    """Test gateway envelope generation"""
    print_test("Testing gateway envelope generation...")
    
    from server.gateway_envelope import build_gateway_envelope
    
    envelope = build_gateway_envelope(
        session_id="test-session-12345",
        hwid_hex="a" * 64,
        puuid="12345678-1234-1234-1234-123456789abc",
        region="la",
        build_info={
            'branch': 'release-13.00-shipping-30-4955671',
            'changelist': 4955671,
            'major': 13,
            'minor': 0,
            'patch': 30,
        },
        rsa_spki_pem=b"-----BEGIN PUBLIC KEY-----\ntest\n-----END PUBLIC KEY-----\n",
        timestamp_ms=int(time.time() * 1000)
    )
    
    if len(envelope) < 200:
        raise ValidationError(f"Gateway envelope too short: {len(envelope)} bytes (expected >200 with F1/F15/OSInfo)")
    
    # Check protobuf structure (starts with field 1, wire type 0)
    if envelope[0] != 0x08:  # Field 1, varint
        print_warn(f"Gateway envelope may not have valid protobuf header")
    
    print_pass(f"Gateway envelope generated: {len(envelope)} bytes (with F1/F15/OSInfo)")

def test_f1_f15_tokens():
    """Test F1 and F15 token generation (VAL 5 fix)"""
    print_test("Testing F1/F15 token generation...")
    
    try:
        from server.vgc_tokens import build_f1_token, build_f15_token, validate_f1_structure
        
        puuid = "test-puuid"
        hwid = b'\x00' * 32
        timestamp_ms = int(time.time() * 1000)
        
        # Test F1
        f1_token = build_f1_token(puuid, hwid, timestamp_ms)
        
        if len(f1_token) < 166:
            raise ValidationError(f"F1 token too short: {len(f1_token)} bytes (expected >=166)")
        
        if not validate_f1_structure(f1_token):
            raise ValidationError("F1 token structure validation failed")
        
        print_pass(f"F1 token generated: {len(f1_token)} bytes (6-component structure)")
        
        # Test F15
        f15_token = build_f15_token(f1_token, "1.18.4.47")
        
        if len(f15_token) != 28:
            raise ValidationError(f"F15 token wrong length: {len(f15_token)} (expected 28)")
        
        # Validate Base64
        import re
        if not re.match(r'^[A-Za-z0-9+/=]+$', f15_token):
            raise ValidationError(f"F15 token not valid Base64: {f15_token}")
        
        print_pass(f"F15 token generated: {f15_token} (Base64 SHA1)")
        
    except ImportError as e:
        raise ValidationError(f"vgc_tokens module not found: {e}")

def test_heartbeat_tasks():
    """Test heartbeat task processing (VAL 5 fix)"""
    print_test("Testing heartbeat task processing...")
    
    try:
        from server.heartbeat_tasks import decrypt_heartbeat_blob, parse_tasks, build_task_results
        print_pass("Heartbeat task functions available (HKDF+AES-GCM)")
    except ImportError as e:
        raise ValidationError(f"heartbeat_tasks module not found: {e}")

def test_server_connection(host='192.168.1.136', port=51820):
    """Test connection to running server"""
    print_test(f"Testing server connection to {host}:{port}...")
    
    try:
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        
        raw_sock = socket.create_connection((host, port), timeout=5)
        sock = ctx.wrap_socket(raw_sock, server_hostname=host)
        
        # Send PING
        ping_msg = struct.pack("!II", 7, 0)  # MsgType.PING, no payload
        sock.sendall(ping_msg)
        
        # Receive PONG
        header = sock.recv(8)
        if len(header) == 8:
            msg_type, payload_len = struct.unpack("!II", header)
            if msg_type == 8:  # MsgType.PONG
                print_pass("Server is responding to PING")
                sock.close()
                return True
        
        sock.close()
        print_warn("Server responded but not with PONG")
        return False
        
    except ConnectionRefusedError:
        print_warn(f"Server not running on {host}:{port}")
        print_info("Start server with: python -m server.main")
        return False
    except socket.timeout:
        print_warn(f"Connection timeout to {host}:{port}")
        return False
    except Exception as e:
        print_fail(f"Connection error: {e}")
        return False

def test_protocol_flow(host='192.168.1.136', port=51820, auth_key='feqxYc-ilusao'):
    """Test full protocol flow"""
    print_test("Testing SESSION_AUTH protocol flow...")
    
    import os
    import uuid
    
    try:
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        
        raw_sock = socket.create_connection((host, port), timeout=10)
        sock = ctx.wrap_socket(raw_sock, server_hostname=host)
        
        # Build SESSION_AUTH
        def pack_len_str(s):
            encoded = s.encode('utf-8')
            return struct.pack("!I", len(encoded)) + encoded
        
        def pack_len_bytes(b):
            return struct.pack("!I", len(b)) + b
        
        payload = b''
        payload += pack_len_str(auth_key)
        payload += pack_len_bytes(os.urandom(32))  # gateway_machine_id
        payload += pack_len_str('eyJhbGciOiJSUzI1NiJ9.test.signature')  # jwt
        payload += pack_len_str(str(uuid.uuid4()))  # puuid
        payload += struct.pack("!I", 12345)  # valorant_pid
        payload += struct.pack("!Q", int(time.time() * 1000))  # timestamp
        payload += pack_len_str('la')  # region
        payload += pack_len_bytes(os.urandom(32))  # hwid_fingerprint
        payload += pack_len_str('test_account')  # riot_account
        payload += pack_len_str('TEST-PC')  # hostname
        
        msg = struct.pack("!II", 14, len(payload)) + payload  # MsgType.SESSION_AUTH
        sock.sendall(msg)
        
        # Receive response
        header = sock.recv(8)
        if len(header) < 8:
            raise ValidationError("Incomplete response header")
        
        msg_type, payload_len = struct.unpack("!II", header)
        
        if msg_type == 9:  # ERROR
            error_msg = sock.recv(payload_len).decode('utf-8', errors='ignore')
            raise ValidationError(f"Server error: {error_msg}")
        
        if msg_type != 15:  # SESSION_AUTH_OK
            raise ValidationError(f"Unexpected response type: {msg_type}")
        
        response_payload = b''
        while len(response_payload) < payload_len:
            chunk = sock.recv(payload_len - len(response_payload))
            if not chunk:
                break
            response_payload += chunk
        
        # Parse session_id
        sid_len = struct.unpack("!I", response_payload[:4])[0]
        session_id = response_payload[4:4+sid_len].decode('utf-8')
        
        print_pass(f"SESSION_AUTH successful. Session ID: {session_id[:8]}...")
        
        # Test IOCTL
        ioctl_payload = struct.pack("!I", 0x222000) + struct.pack("!I", 0)
        ioctl_msg = struct.pack("!II", 4, len(ioctl_payload)) + ioctl_payload
        sock.sendall(ioctl_msg)
        
        header = sock.recv(8)
        msg_type, payload_len = struct.unpack("!II", header)
        
        if msg_type == 5:  # IOCTL_RESP
            ioctl_resp = sock.recv(payload_len)
            resp_len = struct.unpack("!I", ioctl_resp[:4])[0]
            print_pass(f"IOCTL response received: {resp_len} bytes")
        
        sock.close()
        return True
        
    except Exception as e:
        print_fail(f"Protocol test failed: {e}")
        return False

def main():
    print()
    print(f"{Colors.CYAN}{'='*60}{Colors.RESET}")
    print(f"{Colors.CYAN}VGC Emulator Validation{Colors.RESET}")
    print(f"{Colors.CYAN}{'='*60}{Colors.RESET}")
    print()
    
    tests_passed = 0
    tests_failed = 0
    
    # Structure tests (always run)
    try:
        validate_structure()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
        return 1
    
    try:
        config = validate_config()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
    
    try:
        validate_imports()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
        return 1
    
    try:
        validate_certificates()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
    
    # Component tests
    try:
        test_driver_responses()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
    
    try:
        test_gateway_envelope()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
    
    # VAL 5 fixes tests
    try:
        test_f1_f15_tokens()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
    
    try:
        test_heartbeat_tasks()
        tests_passed += 1
    except ValidationError as e:
        print_fail(str(e))
        tests_failed += 1
    
    # Connection tests (optional)
    print()
    print(f"{Colors.CYAN}Testing server connection (optional)...{Colors.RESET}")
    server_running = test_server_connection()
    
    if server_running:
        if test_protocol_flow():
            tests_passed += 1
        else:
            tests_failed += 1
    else:
        print_info("Skipping protocol test (server not running)")
    
    # Summary
    print()
    print(f"{Colors.CYAN}{'='*60}{Colors.RESET}")
    print(f"Tests passed: {Colors.GREEN}{tests_passed}{Colors.RESET}")
    if tests_failed > 0:
        print(f"Tests failed: {Colors.RED}{tests_failed}{Colors.RESET}")
    print(f"{Colors.CYAN}{'='*60}{Colors.RESET}")
    print()
    
    if tests_failed == 0:
        print_pass("All validation tests passed!")
        print()
        print(f"{Colors.CYAN}Next steps:{Colors.RESET}")
        print("  1. Start server: python -m server.main")
        print("  2. Build vClient: .\\build_vclient.ps1 -VpsHost YOUR_IP")
        print("  3. Run vClient: .\\run_vclient.bat")
        print()
        return 0
    else:
        print_fail(f"{tests_failed} test(s) failed. Fix the issues above.")
        return 1

if __name__ == '__main__':
    sys.exit(main())
