from __future__ import annotations

import base64
import json
import re
from dataclasses import dataclass
from typing import Any, Dict, Optional


def _b64url_decode(segment: str) -> str:
    pad = "=" * ((4 - len(segment) % 4) % 4)
    raw = base64.urlsafe_b64decode(segment + pad)
    return raw.decode("utf-8", errors="replace")


def decode_jwt_payload(jwt: str) -> Dict[str, Any]:
    parts = jwt.split(".")
    if len(parts) < 2:
        return {}
    try:
        return json.loads(_b64url_decode(parts[1]))
    except (json.JSONDecodeError, ValueError):
        return {}


def shard_from_jwt(jwt: str) -> str:
    payload = decode_jwt_payload(jwt)
    r = str(payload.get("r", "")).lower()
    if r in {"br", "br1", "la1"}:
        return "la"
    if r in {"la", "na", "eu", "ap", "kr"}:
        return r
    cty = str(payload.get("cty", payload.get("lcty", ""))).lower()
    if cty in {"usa", "can"}:
        return "na"
    if cty in {"kor", "kr"}:
        return "ap"
    if cty in {"bra", "arg", "chl", "col", "mex"}:
        return "la"
    if cty in {"deu", "fra", "gbr", "esp", "ita"}:
        return "eu"
    if cty in {"jpn", "aus", "sgp"}:
        return "ap"
    return ""


def account_from_jwt(jwt: str) -> str:
    payload = decode_jwt_payload(jwt)
    for key in ("sub", "acct", "name"):
        val = payload.get(key)
        if isinstance(val, str) and val:
            return val
    return ""


def puuid_from_jwt(jwt: str) -> str:
    payload = decode_jwt_payload(jwt)
    for key in ("sub", "puuid"):
        val = payload.get(key)
        if isinstance(val, str) and re.fullmatch(
            r"[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}",
            val,
        ):
            return val
    m = re.search(
        r"[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}",
        jwt,
    )
    return m.group(0) if m else ""
