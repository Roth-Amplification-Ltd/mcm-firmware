#!/usr/bin/env python3
"""Validate and decode MCM SPI snapshot packets from hexadecimal text lines."""

from __future__ import annotations

import argparse
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable

SYNC = 0xA5
VERSION = 0x01
PACKET_SIZE = 8
CONTROL_COUNT = 6

PARAM_STATE = 0x01
BUTTON_STATE = 0x02
SNAPSHOT_BEGIN = 0x10
SNAPSHOT_END = 0x11


def crc8(data: Iterable[int]) -> int:
    crc = 0
    for value in data:
        crc ^= value
        for _ in range(8):
            crc = ((crc << 1) ^ 0x07) & 0xFF if crc & 0x80 else (crc << 1) & 0xFF
    return crc


def parse_packet(line: str) -> list[int]:
    clean = line.split("#", 1)[0].strip()
    if not clean:
        return []
    values = [int(token, 16) for token in clean.replace(",", " ").split()]
    if len(values) != PACKET_SIZE:
        raise ValueError(f"expected {PACKET_SIZE} bytes, got {len(values)}: {line.rstrip()}")
    if any(value < 0 or value > 0xFF for value in values):
        raise ValueError("packet byte outside 0x00..0xFF")
    if values[0] != SYNC:
        raise ValueError(f"bad sync 0x{values[0]:02X}")
    if values[1] != VERSION:
        raise ValueError(f"unsupported version 0x{values[1]:02X}")
    expected = crc8(values[:7])
    if values[7] != expected:
        raise ValueError(f"bad CRC 0x{values[7]:02X}; expected 0x{expected:02X}")
    return values


def decode_i24(data0: int, data1: int, data2: int) -> int:
    raw = data0 | (data1 << 8) | (data2 << 16)
    return raw - 0x1000000 if raw & 0x800000 else raw


@dataclass
class Snapshot:
    sequence_id: int | None = None
    status: int = 0
    encoders: dict[int, int] = field(default_factory=dict)
    pressed: int | None = None
    long_held: int | None = None

    def consume(self, packet: list[int]) -> bool:
        message_type = packet[2]
        index = packet[3]

        if message_type == SNAPSHOT_BEGIN:
            if index != CONTROL_COUNT:
                raise ValueError(f"begin count is {index}, expected {CONTROL_COUNT}")
            self.sequence_id = packet[4] | (packet[5] << 8)
            self.status = packet[6]
            return False

        if self.sequence_id is None:
            raise ValueError("state packet received before SNAPSHOT_BEGIN")

        if message_type == PARAM_STATE:
            if index >= CONTROL_COUNT:
                raise ValueError(f"invalid encoder index {index}")
            if index in self.encoders:
                raise ValueError(f"duplicate encoder index {index}")
            self.encoders[index] = decode_i24(packet[4], packet[5], packet[6])
            return False

        if message_type == BUTTON_STATE:
            if index != 0 or packet[6] != CONTROL_COUNT:
                raise ValueError("invalid button-state bank or count")
            if self.pressed is not None:
                raise ValueError("duplicate button-state packet")
            self.pressed = packet[4] & 0x3F
            self.long_held = packet[5] & 0x3F
            return False

        if message_type == SNAPSHOT_END:
            end_sequence = packet[4] | (packet[5] << 8)
            if index != CONTROL_COUNT:
                raise ValueError("invalid snapshot end count")
            if end_sequence != self.sequence_id:
                raise ValueError("snapshot sequence mismatch")
            if packet[6] != self.status:
                raise ValueError("snapshot status mismatch")
            if set(self.encoders) != set(range(CONTROL_COUNT)):
                raise ValueError("snapshot does not contain all six encoder indexes")
            if self.pressed is None or self.long_held is None:
                raise ValueError("snapshot does not contain button state")
            return True

        raise ValueError(f"unexpected message type 0x{message_type:02X}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path, help="text file containing one 8-byte hex packet per line")
    args = parser.parse_args()

    snapshot = Snapshot()
    complete = False
    for line_number, line in enumerate(args.input.read_text(encoding="utf-8").splitlines(), 1):
        try:
            packet = parse_packet(line)
            if not packet:
                continue
            if packet[2] == 0x81:  # Example files may include the request packet.
                continue
            complete = snapshot.consume(packet)
        except ValueError as error:
            raise SystemExit(f"line {line_number}: {error}") from error

    if not complete:
        raise SystemExit("input ended before a complete valid snapshot")

    print(f"snapshot sequence: {snapshot.sequence_id}")
    print(f"status flags:      0x{snapshot.status:02X}")
    for index in range(CONTROL_COUNT):
        pressed = bool(snapshot.pressed & (1 << index))
        long_held = bool(snapshot.long_held & (1 << index))
        print(
            f"EN{index + 1}: value={snapshot.encoders[index]:8d} "
            f"button={'pressed' if pressed else 'released':8s} "
            f"long={'yes' if long_held else 'no'}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
