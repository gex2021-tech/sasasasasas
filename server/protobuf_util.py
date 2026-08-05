"""Shared protobuf wire-format helpers used by the VGC emulation modules."""
from __future__ import annotations

from typing import Tuple

WIRE_VARINT = 0
WIRE_FIXED64 = 1
WIRE_LENGTH_DELIMITED = 2
WIRE_FIXED32 = 5


def encode_varint(value: int) -> bytes:
    """Encode integer as protobuf varint"""
    buf = bytearray()
    while value > 0x7F:
        buf.append((value & 0x7F) | 0x80)
        value >>= 7
    buf.append(value & 0x7F)
    return bytes(buf)


def read_varint(data: bytes, offset: int) -> Tuple[int, int]:
    """Read varint from bytes at offset, return (value, new_offset)"""
    value = 0
    shift = 0
    while offset < len(data):
        byte = data[offset]
        offset += 1
        value |= (byte & 0x7F) << shift
        if not (byte & 0x80):
            break
        shift += 7
    return value, offset


def encode_field(field_num: int, wire_type: int, data: bytes) -> bytes:
    """Encode protobuf field: (field_num << 3) | wire_type + data

    Length-delimited fields are prefixed with their length.
    """
    result = bytearray(encode_varint((field_num << 3) | wire_type))
    if wire_type == WIRE_LENGTH_DELIMITED:
        result.extend(encode_varint(len(data)))
    result.extend(data)
    return bytes(result)


def encode_varint_field(field_num: int, value: int) -> bytes:
    return encode_field(field_num, WIRE_VARINT, encode_varint(value))


def encode_bytes_field(field_num: int, data: bytes) -> bytes:
    return encode_field(field_num, WIRE_LENGTH_DELIMITED, data)


def encode_string_field(field_num: int, value: str) -> bytes:
    return encode_bytes_field(field_num, value.encode("utf-8"))
