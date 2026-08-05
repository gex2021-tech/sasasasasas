"""HWID spoofing — generate realistic hardware fingerprints for VGC sessions."""
from __future__ import annotations

import hashlib
import json
import logging
import random
import uuid
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Optional

log = logging.getLogger("hwid_spoofer")

# ---------------------------------------------------------------------------
#  Realistic hardware pools (sourced from common consumer hardware)
# ---------------------------------------------------------------------------

_BIOS_VERSIONS = [
    "American Megatrends Inc. F20",
    "American Megatrends Inc. 2.17.1249",
    "American Megatrends Inc. 5.19",
    "Phoenix SecureCore Tiano NB 04.06.04",
    "Dell Inc. 2.24.0",
    "Dell Inc. A17",
    "HP F.75",
    "HP S71 Ver. 01.12.00",
    "Lenovo N1CET76W (1.54)",
    "Lenovo GKCN58WW",
    "ASUS ROG BIOS 1801",
    "MSI E7D73IMS.130",
    "Gigabyte F23a",
    "Insyde Corp. V1.14",
    "ASRock P5.10",
    "BIOSTAR B550MH 3.0",
    "EVGA 1.17",
]

_CPU_MODELS = [
    "Intel(R) Core(TM) i9-14900K",
    "Intel(R) Core(TM) i9-13900K",
    "Intel(R) Core(TM) i7-13700K",
    "Intel(R) Core(TM) i7-14700KF",
    "Intel(R) Core(TM) i7-12700K",
    "Intel(R) Core(TM) i5-13600K",
    "Intel(R) Core(TM) i5-14600KF",
    "Intel(R) Core(TM) i5-12400F",
    "AMD Ryzen 9 7950X 16-Core Processor",
    "AMD Ryzen 9 7900X 12-Core Processor",
    "AMD Ryzen 7 7800X3D 8-Core Processor",
    "AMD Ryzen 7 5800X 8-Core Processor",
    "AMD Ryzen 5 7600X 6-Core Processor",
    "AMD Ryzen 5 5600X 6-Core Processor",
    "AMD Ryzen 5 5600 6-Core Processor",
    "Intel(R) Core(TM) i9-12900KS",
    "AMD Ryzen 9 5950X 16-Core Processor",
]

_GPU_BRANDS_MODELS = [
    ("NVIDIA", "NVIDIA GeForce RTX 4090"),
    ("NVIDIA", "NVIDIA GeForce RTX 4080 SUPER"),
    ("NVIDIA", "NVIDIA GeForce RTX 4070 Ti SUPER"),
    ("NVIDIA", "NVIDIA GeForce RTX 4070"),
    ("NVIDIA", "NVIDIA GeForce RTX 4060 Ti"),
    ("NVIDIA", "NVIDIA GeForce RTX 3090 Ti"),
    ("NVIDIA", "NVIDIA GeForce RTX 3080"),
    ("NVIDIA", "NVIDIA GeForce RTX 3070"),
    ("NVIDIA", "NVIDIA GeForce RTX 3060 Ti"),
    ("AMD", "AMD Radeon RX 7900 XTX"),
    ("AMD", "AMD Radeon RX 7900 XT"),
    ("AMD", "AMD Radeon RX 7800 XT"),
    ("AMD", "AMD Radeon RX 7600"),
    ("AMD", "AMD Radeon RX 6800 XT"),
    ("AMD", "AMD Radeon RX 6700 XT"),
]

# CPU brand inferred from model name
_CPU_CORE_COUNTS = {
    "i9-14900K": 24, "i9-13900K": 24, "i9-12900KS": 24,
    "i7-13700K": 16, "i7-14700KF": 20, "i7-12700K": 12,
    "i5-13600K": 14, "i5-14600KF": 14, "i5-12400F": 12,
    "7950X": 32, "7900X": 24, "7800X3D": 16,
    "5800X": 16, "7600X": 12, "5600X": 12, "5600": 12,
    "5950X": 32,
}


def _core_count_for(cpu_model: str) -> int:
    for tag, count in _CPU_CORE_COUNTS.items():
        if tag in cpu_model:
            return count
    return random.choice([8, 12, 16])


def _cpu_brand(cpu_model: str) -> str:
    if "Intel" in cpu_model:
        return "Intel"
    if "AMD" in cpu_model:
        return "AMD"
    return "Unknown"


# ---------------------------------------------------------------------------
#  HwidProfile
# ---------------------------------------------------------------------------

@dataclass
class HwidProfile:
    bios_version: str
    cpu_model: str
    cpu_brand: str
    cpu_logical_count: int
    gpu_brand: str
    gpu_model: str
    volume_serial: str
    machine_guid: str
    composite_hash: bytes  # 32-byte SHA-256

    def hash_hex(self) -> str:
        return self.composite_hash.hex()


def _compute_composite(bios: str, cpu: str, vol: str, guid: str) -> bytes:
    """Match vClient GetRealHwid() composite format."""
    composite = f"BIOS:{bios}|CPU:{cpu}|VOL:{vol}|MGUID:{guid}"
    return hashlib.sha256(composite.encode("utf-8")).digest()


def _build_profile(
    bios: str, cpu: str, gpu_brand: str, gpu_model: str, vol: str, guid: str
) -> HwidProfile:
    """Assemble a profile, deriving the CPU/composite fields from the raw values."""
    return HwidProfile(
        bios_version=bios,
        cpu_model=cpu,
        cpu_brand=_cpu_brand(cpu),
        cpu_logical_count=_core_count_for(cpu),
        gpu_brand=gpu_brand,
        gpu_model=gpu_model,
        volume_serial=vol,
        machine_guid=guid,
        composite_hash=_compute_composite(bios, cpu, vol, guid),
    )


def generate_hwid() -> HwidProfile:
    """Generate a fully random but realistic HWID profile."""
    gpu_brand, gpu_model = random.choice(_GPU_BRANDS_MODELS)
    return _build_profile(
        bios=random.choice(_BIOS_VERSIONS),
        cpu=random.choice(_CPU_MODELS),
        gpu_brand=gpu_brand,
        gpu_model=gpu_model,
        vol=f"{random.randint(0, 0xFFFFFFFF):08X}",
        guid=str(uuid.uuid4()),
    )


def rotate_hwid(previous: HwidProfile) -> HwidProfile:
    """Rotate HWID — change 2-3 fields to avoid pattern detection.

    Keeps some fields from previous to look like a plausible hardware
    change (BIOS update, new GPU, etc.) rather than a full system swap.
    """
    fields_to_change = random.sample(
        ["bios", "cpu", "gpu", "vol", "guid"], k=random.randint(2, 3)
    )

    bios = random.choice(_BIOS_VERSIONS) if "bios" in fields_to_change else previous.bios_version
    cpu = random.choice(_CPU_MODELS) if "cpu" in fields_to_change else previous.cpu_model

    if "gpu" in fields_to_change:
        gpu_brand, gpu_model = random.choice(_GPU_BRANDS_MODELS)
    else:
        gpu_brand, gpu_model = previous.gpu_brand, previous.gpu_model

    vol = f"{random.randint(0, 0xFFFFFFFF):08X}" if "vol" in fields_to_change else previous.volume_serial
    guid = str(uuid.uuid4()) if "guid" in fields_to_change else previous.machine_guid

    profile = _build_profile(bios, cpu, gpu_brand, gpu_model, vol, guid)
    log.info(
        "hwid rotated: changed=%s hash=%s",
        ",".join(fields_to_change),
        profile.hash_hex()[:16],
    )
    return profile


# ---------------------------------------------------------------------------
#  Persistence
# ---------------------------------------------------------------------------

def save_hwid(profile: HwidProfile, path: Path) -> None:
    """Save HWID profile to JSON for cross-session persistence."""
    path.parent.mkdir(parents=True, exist_ok=True)
    data = asdict(profile)
    data["composite_hash"] = profile.composite_hash.hex()
    path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    log.info("hwid saved to %s", path)


def load_hwid(path: Path) -> Optional[HwidProfile]:
    """Load HWID profile from JSON. Returns None if file missing/corrupt."""
    if not path.exists():
        return None
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
        data["composite_hash"] = bytes.fromhex(data["composite_hash"])
        return HwidProfile(**data)
    except (json.JSONDecodeError, KeyError, ValueError) as e:
        log.warning("failed to load hwid from %s: %s", path, e)
        return None


def get_or_create_hwid(persist_path: Path, rotate: bool = False) -> HwidProfile:
    """Load existing HWID or generate a new one. Optionally rotate."""
    existing = load_hwid(persist_path)
    if existing and not rotate:
        log.info("hwid loaded: hash=%s", existing.hash_hex()[:16])
        return existing
    if existing and rotate:
        profile = rotate_hwid(existing)
    else:
        profile = generate_hwid()
        log.info("hwid generated: hash=%s", profile.hash_hex()[:16])
    save_hwid(profile, persist_path)
    return profile
