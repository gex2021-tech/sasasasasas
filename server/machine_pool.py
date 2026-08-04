"""Machine Pool Generator - 500 Hardware Profiles for Anti-Fingerprinting

Based on paid emulator logs: Generates 500 fake machine profiles in memory,
selects one randomly per session to avoid hardware consistency detection.

Each profile includes:
- BIOS vendor/version
- CPU brand/model/cores
- GPU brand/model
- Volume serial numbers
- Motherboard info
"""
import random
import hashlib
from typing import Dict, List, Tuple
from dataclasses import dataclass


@dataclass
class MachineProfile:
    """Single fake machine profile"""
    bios_vendor: str
    bios_version: str
    cpu_brand: str
    cpu_model: str
    cpu_cores: int
    cpu_logical: int
    gpu_brand: str
    gpu_model: str
    volume_serial: str
    motherboard: str
    hostname: str
    profile_hash: str  # SHA1 of all fields for consistency


class MachinePool:
    """Generate and manage 500 fake machine profiles"""
    
    # Realistic BIOS vendors (from real gaming PCs)
    BIOS_VENDORS = [
        "American Megatrends Inc.",
        "Phoenix Technologies",
        "Dell Inc.",
        "HP",
        "Lenovo",
        "ASUS",
        "Gigabyte Technology",
        "MSI",
        "ASRock",
        "Biostar"
    ]
    
    # CPU configurations (brand, model, cores, logical)
    CPU_CONFIGS = [
        ("AMD", "Ryzen 9 7950X", 16, 32),
        ("AMD", "Ryzen 9 5950X", 16, 32),
        ("AMD", "Ryzen 7 7800X3D", 8, 16),
        ("AMD", "Ryzen 7 5800X3D", 8, 16),
        ("AMD", "Ryzen 7 5700X", 8, 16),
        ("AMD", "Ryzen 5 7600X", 6, 12),
        ("AMD", "Ryzen 5 5600X", 6, 12),
        ("Intel", "Core i9-14900K", 24, 32),
        ("Intel", "Core i9-13900K", 24, 32),
        ("Intel", "Core i7-14700K", 20, 28),
        ("Intel", "Core i7-13700K", 16, 24),
        ("Intel", "Core i5-14600K", 14, 20),
        ("Intel", "Core i5-13600K", 14, 20),
        ("Intel", "Core i5-12600K", 10, 16),
    ]
    
    # GPU configurations (brand, model)
    GPU_CONFIGS = [
        ("NVIDIA", "GeForce RTX 4090"),
        ("NVIDIA", "GeForce RTX 4080"),
        ("NVIDIA", "GeForce RTX 4070 Ti"),
        ("NVIDIA", "GeForce RTX 4070"),
        ("NVIDIA", "GeForce RTX 4060 Ti"),
        ("NVIDIA", "GeForce RTX 4060"),
        ("NVIDIA", "GeForce RTX 3090 Ti"),
        ("NVIDIA", "GeForce RTX 3090"),
        ("NVIDIA", "GeForce RTX 3080 Ti"),
        ("NVIDIA", "GeForce RTX 3080"),
        ("NVIDIA", "GeForce RTX 3070 Ti"),
        ("NVIDIA", "GeForce RTX 3070"),
        ("NVIDIA", "GeForce RTX 3060 Ti"),
        ("NVIDIA", "GeForce RTX 3060"),
        ("AMD", "Radeon RX 7900 XTX"),
        ("AMD", "Radeon RX 7900 XT"),
        ("AMD", "Radeon RX 7800 XT"),
        ("AMD", "Radeon RX 7700 XT"),
        ("AMD", "Radeon RX 6950 XT"),
        ("AMD", "Radeon RX 6900 XT"),
        ("AMD", "Radeon RX 6800 XT"),
    ]
    
    # Motherboard manufacturers
    MOBO_MANUFACTURERS = [
        "ASUS ROG MAXIMUS",
        "ASUS TUF GAMING",
        "ASUS PRIME",
        "Gigabyte AORUS",
        "Gigabyte GAMING",
        "MSI MEG",
        "MSI MPG",
        "MSI MAG",
        "ASRock Taichi",
        "ASRock Phantom Gaming",
        "Dell OptiPlex",
        "HP Pavilion"
    ]
    
    def __init__(self, pool_size: int = 500):
        self.pool_size = pool_size
        self.profiles: List[MachineProfile] = []
        self._generate_pool()
    
    def _generate_pool(self) -> None:
        """Generate all 500 machine profiles"""
        for i in range(self.pool_size):
            profile = self._generate_single_profile(i)
            self.profiles.append(profile)
    
    def _generate_single_profile(self, index: int) -> MachineProfile:
        """Generate one machine profile with deterministic randomness"""
        # Use index as seed for reproducibility
        rng = random.Random(index)
        
        # BIOS
        bios_vendor = rng.choice(self.BIOS_VENDORS)
        bios_version = f"{rng.choice(['F', 'P', 'A'])}{rng.randint(10, 99)}"
        
        # CPU
        cpu_idx = rng.randint(0, len(self.CPU_CONFIGS) - 1)
        cpu_brand, cpu_model, cpu_cores, cpu_logical = self.CPU_CONFIGS[cpu_idx]
        
        # GPU
        gpu_idx = rng.randint(0, len(self.GPU_CONFIGS) - 1)
        gpu_brand, gpu_model = self.GPU_CONFIGS[gpu_idx]
        
        # Volume serial (format: XXXX-XXXX hex)
        vol_high = rng.randint(0x1000, 0xFFFF)
        vol_low = rng.randint(0x1000, 0xFFFF)
        volume_serial = f"{vol_high:04X}-{vol_low:04X}"
        
        # Motherboard
        mobo = f"{rng.choice(self.MOBO_MANUFACTURERS)} {rng.choice(['Z790', 'B650', 'X670', 'Z690', 'B550', 'X570'])}"
        
        # Hostname (DESKTOP-XXXXXXXX format)
        hostname_suffix = ''.join(rng.choices('0123456789ABCDEF', k=8))
        hostname = f"DESKTOP-{hostname_suffix}"
        
        # Create hash for consistency checking
        profile_data = f"{bios_vendor}{bios_version}{cpu_model}{gpu_model}{volume_serial}{mobo}{hostname}"
        profile_hash = hashlib.sha1(profile_data.encode()).hexdigest()[:16]
        
        return MachineProfile(
            bios_vendor=bios_vendor,
            bios_version=bios_version,
            cpu_brand=cpu_brand,
            cpu_model=cpu_model,
            cpu_cores=cpu_cores,
            cpu_logical=cpu_logical,
            gpu_brand=gpu_brand,
            gpu_model=gpu_model,
            volume_serial=volume_serial,
            motherboard=mobo,
            hostname=hostname,
            profile_hash=profile_hash
        )
    
    def select_random(self, seed: int = None) -> Tuple[int, MachineProfile]:
        """Select a random profile from the pool
        
        Args:
            seed: Optional seed for reproducible selection
            
        Returns:
            (index, MachineProfile) tuple
        """
        if seed is not None:
            rng = random.Random(seed)
            idx = rng.randint(0, self.pool_size - 1)
        else:
            idx = random.randint(0, self.pool_size - 1)
        
        return idx, self.profiles[idx]
    
    def get_profile(self, index: int) -> MachineProfile:
        """Get specific profile by index"""
        if 0 <= index < self.pool_size:
            return self.profiles[index]
        raise IndexError(f"Profile index {index} out of range (0-{self.pool_size-1})")
    
    def export_for_session(self, index: int) -> Dict:
        """Export profile in format ready for session auth
        
        Returns dict matching SessionAuthData fields
        """
        profile = self.get_profile(index)
        
        return {
            "cpu_brand": profile.cpu_brand,
            "cpu_model": profile.cpu_model,
            "cpu_logical_count": profile.cpu_logical,
            "gpu_brand": profile.gpu_brand,
            "gpu_model": profile.gpu_model,
            "hostname": profile.hostname,
            "bios_info": f"{profile.bios_vendor} {profile.bios_version}",
            "motherboard": profile.motherboard,
            "volume_serial": profile.volume_serial,
            "profile_hash": profile.profile_hash
        }
    
    def validate_consistency(self, index: int, provided_data: Dict) -> bool:
        """Validate that provided hardware data matches stored profile"""
        try:
            profile = self.get_profile(index)
            
            # Check critical fields match
            if provided_data.get("cpu_model") != profile.cpu_model:
                return False
            if provided_data.get("gpu_model") != profile.gpu_model:
                return False
            if provided_data.get("hostname") != profile.hostname:
                return False
                
            return True
        except IndexError:
            return False


# Singleton instance for global access
_global_pool: MachinePool = None


def get_machine_pool() -> MachinePool:
    """Get or create global machine pool"""
    global _global_pool
    if _global_pool is None:
        _global_pool = MachinePool(500)
    return _global_pool


def select_machine_for_session(session_seed: int = None) -> Tuple[int, Dict]:
    """Select machine profile for new session
    
    Args:
        session_seed: Optional seed (e.g., from session_id hash)
        
    Returns:
        (index, profile_dict) tuple
    """
    pool = get_machine_pool()
    idx, profile = pool.select_random(session_seed)
    return idx, pool.export_for_session(idx)
