import struct
import argparse
import sys
import time
import os
import uuid
from pathlib import Path

import yaml

sys.path.insert(0, str(Path(__file__).resolve().parent))

from server.net_util import connect_tls, recv_message
from server.protocol import IOCTL_VGK, MsgType, pack, pack_session_auth

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="192.168.1.136")
    parser.add_argument("--port", type=int, default=51820)
    args = parser.parse_args()
    
    auth_key = 'feqxYc-ilusao'
    try:
        with open("config.yaml", "r") as f:
            cfg = yaml.safe_load(f)
            if "server" in cfg and "auth_key" in cfg["server"]:
                auth_key = cfg["server"]["auth_key"]
    except Exception:
        pass

    print(f"Connecting to {args.host}:{args.port}...")
    try:
        sock = connect_tls(args.host, args.port)
    except Exception as e:
        print(f"\033[91m[-] Connection failed: {e}\033[0m")
        return

    print("\033[92m[+] Connected.\033[0m")
    
    session_auth_payload = pack_session_auth(
        auth_key=auth_key,
        gateway_machine_id=os.urandom(32),
        jwt='eyJhbGciOiJSUzI1NiJ9.eyJ0ZXN0IjoidGVzdCJ9.test_signature',
        puuid=str(uuid.uuid4()),
        valorant_pid=12345,
        client_ts_ms=int(time.time() * 1000),
        region='la',
        hwid_fingerprint=os.urandom(32),
        riot_account='test_account',
        hostname='TEST-PC'
    )
    
    sock.sendall(pack(MsgType.SESSION_AUTH, session_auth_payload))
    print("[*] Sent SESSION_AUTH. Waiting for response...")
    
    msg_type, payload = recv_message(sock) or (None, b"")
    if msg_type == MsgType.SESSION_AUTH_OK:
        sid_len = struct.unpack_from("!I", payload, 0)[0]
        session_id = payload[4:4+sid_len].decode("utf-8")
        print(f"\033[92m[+] SESSION_AUTH_OK received. Session ID: {session_id}\033[0m")
    elif msg_type == MsgType.ERROR:
        err_msg = payload.decode("utf-8", errors="ignore")
        print(f"\033[91m[-] Server returned error: {err_msg}\033[0m")
        return
    else:
        print(f"\033[91m[-] Unexpected response type: {msg_type}\033[0m")
        return

    sock.sendall(pack(MsgType.PING))
    print("[*] Sent PING...")
    
    msg_type, payload = recv_message(sock) or (None, b"")
    if msg_type == MsgType.PONG:
        print("\033[92m[+] PONG received.\033[0m")
    else:
        print(f"\033[91m[-] Expected PONG, got {msg_type}\033[0m")
        return
        
    ioctl_payload = struct.pack("!I", IOCTL_VGK) + struct.pack("!I", 0)
    sock.sendall(pack(MsgType.IOCTL, ioctl_payload))
    print("[*] Sent IOCTL...")
    
    msg_type, payload = recv_message(sock) or (None, b"")
    if msg_type == MsgType.IOCTL_RESP:
        print("\033[92m[+] IOCTL_RESP received.\033[0m")
    else:
        print(f"\033[91m[-] Expected IOCTL_RESP, got {msg_type}\033[0m")
        
    sock.close()

if __name__ == "__main__":
    main()
