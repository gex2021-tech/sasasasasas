"""
Demo del Loader - Sin necesidad de Valorant
Simula todas las etapas para ver la UI
"""
from emulator_loader import EmulatorLoader
import threading
import time

class DemoLoader(EmulatorLoader):
    """Version demo que no requiere Valorant"""
    
    def verify_server_connection(self, host, port, timeout=5):
        """Simula verificación de servidor"""
        self.update_status(f"✓ Connecting to {host}:{port}... (DEMO)")
        time.sleep(2)
        return True
    
    def wait_for_game(self, timeout=300):
        """Simula detección de juego (auto después de 5s)"""
        self.update_status("⏳ DEMO MODE: Simulando detección de juego...")
        
        for i in range(5):
            time.sleep(1)
            # Update progress smoothly 30% -> 40%
            progress = 30 + (i * 2)
            self.update_progress(progress, 3)
        
        self.update_status("✓ Juego detectado (DEMO)")
        self.game_detected = True
        return True
    
    def start_vclient(self):
        """Simula inicio de vClient"""
        self.update_status("✓ vClient iniciado (DEMO)")
        self.vclient_running = True
        return True
    
    def establish_heartbeats(self):
        """Simula conexión de heartbeats"""
        self.update_status("✓ Heartbeats establecidos (DEMO)")
        return True
    
    def send_auth_request(self):
        """Simula envío de auth"""
        self.update_status("✓ Auth enviado (DEMO)")
        return True

if __name__ == "__main__":
    print("=" * 60)
    print("VGC EMULATOR LOADER - DEMO MODE")
    print("=" * 60)
    print()
    print("Este modo DEMO simula todas las etapas sin necesitar:")
    print("  - vClient.exe")
    print("  - Servidor corriendo")
    print("  - Valorant instalado")
    print()
    print("Perfecto para ver la UI y probar el flujo!")
    print("=" * 60)
    print()
    
    loader = DemoLoader()
    loader.run()
