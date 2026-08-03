import socket
import ssl
import struct
import argparse
import time
import os
import uuid
import yaml

HEADER = struct.Struct("!II")
IOCTL_VGK = 0x222000

class MsgType:
    IOCTL = 4
    IOCTL_RESP = 5
    PING = 7
    PONG = 8
    ERROR = 9
    SESSION_AUTH = 14
    SESSION_AUTH_OK = 15

def pack(msg_type: int, payload: bytes = b"") -> bytes:
    return HEADER.pack(msg_type, len(payload)) + payload

def pack_len_prefixed(data: bytes) -> bytes:
    return struct.pack("!I", len(data)) + data

def pack_len_prefixed_str(s: str) -> bytes:
    return pack_len_prefixed(s.encode("utf-8"))

def recv_msg(sock):
    header_data = sock.recv(HEADER.size)
    if not header_data:
        return None, None
    if len(header_data) < HEADER.size:
        raise ValueError("Incomplete header")
    msg_type, payload_len = HEADER.unpack(header_data)
    
    payload = b""
    while len(payload) < payload_len:
        chunk = sock.recv(payload_len - len(payload))
        if not chunk:
            break
        payload += chunk
    return msg_type, payload

def pack_session_auth(
    auth_key: str,
    gateway_machine_id: bytes,
    jwt: str,
    puuid: str,
    valorant_pid: int,
    client_ts_ms: int,
    region: str,
    hwid_fingerprint: bytes,
    riot_account: str,
    hostname: str
) -> bytes:
    payload = b""
    payload += pack_len_prefixed_str(auth_key)
    payload += pack_len_prefixed(gateway_machine_id)
    payload += pack_len_prefixed_str(jwt)
    payload += pack_len_prefixed_str(puuid)
    payload += struct.pack("!I", valorant_pid)
    payload += struct.pack("!Q", client_ts_ms)
    payload += pack_len_prefixed_str(region)
    payload += pack_len_prefixed(hwid_fingerprint)
    payload += pack_len_prefixed_str(riot_account)
    payload += pack_len_prefixed_str(hostname)
    return payload

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
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

    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE

    print(f"Connecting to {args.host}:{args.port}...")
    try:
        raw_sock = socket.create_connection((args.host, args.port))
        sock = ctx.wrap_socket(raw_sock, server_hostname=args.host)
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
    
    msg_type, payload = recv_msg(sock)
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
    
    msg_type, payload = recv_msg(sock)
    if msg_type == MsgType.PONG:
        print("\033[92m[+] PONG received.\033[0m")
    else:
        print(f"\033[91m[-] Expected PONG, got {msg_type}\033[0m")
        return
        
    ioctl_payload = struct.pack("!I", IOCTL_VGK) + struct.pack("!I", 0)
    sock.sendall(pack(MsgType.IOCTL, ioctl_payload))
    print("[*] Sent IOCTL...")
    
    msg_type, payload = recv_msg(sock)
    if msg_type == MsgType.IOCTL_RESP:
        print("\033[92m[+] IOCTL_RESP received.\033[0m")
    else:
        print(f"\033[91m[-] Expected IOCTL_RESP, got {msg_type}\033[0m")
        
    sock.close()

if __name__ == "__main__":
    main()
