import socket
import sys

# Forzar UTF-8 en salida estándar para Windows
if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8')

HOST = '127.0.0.1'
PORT = 51820

print(f"Conectando al emulador en {HOST}:{PORT}...")

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        
        request = b"GET /session HTTP/1.1\r\nHost: vgc.live.riotgames.com\r\n\r\n"
        s.sendall(request)
        
        data = s.recv(4096)
        
        if data:
            print("[+] Respuesta recibida del emulador!")
            print(data.decode('utf-8', errors='ignore'))
        else:
            print("[!] Conexion cerrada sin datos.")
except Exception as e:
    print(f"[-] Error: {e}")
