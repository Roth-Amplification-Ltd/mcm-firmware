# MCM SPI Wire Protocol

## Status vocabulary

- **Implemented:** present in the nested protocol source on `main`.
- **Design target:** specified by the coherent snapshot package but not fully implemented.

## Electrical and transfer format

| Property | Value |
|---|---|
| Logic | 3.3 V |
| Mode | SPI Mode 0 (`CPOL=0`, `CPHA=0`) |
| Bit order | MSB-first on the wire |
| Chip select | Active low |
| Frame size | Exactly 8 bytes / 64 clock edges per CS assertion |
| IRQ | Active-high level indicating pending response data |

The master must not assume that the response to a command appears during the same full-duplex frame. The MCM validates and processes a command after the CS-framed receive packet is complete, then queues later response packets.

## Common eight-byte envelope

| Byte | Name | Description |
|---:|---|---|
| 0 | `SYNC` | Always `0xA5` |
| 1 | `VERSION` | Currently `0x01` |
| 2 | `TYPE` | Message ID |
| 3 | `INDEX` | Parameter index, control count, or type-specific selector |
| 4 | `DATA0` | Payload byte 0 / least-significant byte |
| 5 | `DATA1` | Payload byte 1 |
| 6 | `DATA2` | Payload byte 2 / most-significant byte |
| 7 | `CRC8` | CRC-8 of bytes 0 through 6 |

Never transmit an in-memory C/C++ struct directly. Build the eight explicit bytes so compiler padding and host endianness cannot alter the protocol.

## CRC-8

| Property | Value |
|---|---|
| Polynomial | `0x07` (`x^8 + x^2 + x + 1`) |
| Initial value | `0x00` |
| Reflect input | No |
| Reflect output | No |
| Final XOR | `0x00` |
| Covered bytes | 0–6 |

Reference implementation:

```cpp
uint8_t crc8(const uint8_t* data, uint8_t length) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x80u)
                ? static_cast<uint8_t>((crc << 1) ^ 0x07u)
                : static_cast<uint8_t>(crc << 1);
        }
    }
    return crc;
}
```

## Implemented message IDs

### MCM to master

| Name | ID | Payload |
|---|---:|---|
| `MSG_PARAM_STATE` | `0x01` | `INDEX` = parameter; bytes 4–6 = 24-bit value |
| `MSG_SNAPSHOT_BEGIN` | `0x10` | `INDEX` = parameter count; bytes 4–6 reserved in current code |
| `MSG_SNAPSHOT_END` | `0x11` | `INDEX` = parameter count; bytes 4–6 reserved in current code |

### Master to MCM

| Name | ID | Behavior |
|---|---:|---|
| `MSG_CMD_NOP` | `0x80` | Validate, then do nothing |
| `MSG_CMD_GET_SNAPSHOT` | `0x81` | Enqueue BEGIN, one state packet per parameter, END |
| `MSG_CMD_RESET_PARAM` | `0x82` | Reset `INDEX`, then publish its state |
| `MSG_CMD_RESET_ALL` | `0x83` | Reset and publish all parameters |
| `MSG_CMD_SET_LED` | `0x84` | Reserved/stub |
| `MSG_CMD_GET_INFO` | `0x85` | Reserved/stub |
| `MSG_CMD_RESYNC` | `0x86` | Clear pending software events and TX queue, then request a fresh snapshot |

## 24-bit value encoding

Bytes 4–6 are little-endian:

```text
raw24 = DATA0 | DATA1 << 8 | DATA2 << 16
```

For signed interpretation, sign-extend bit 23:

```cpp
int32_t decodeSigned24(const uint8_t* packet) {
    int32_t value = static_cast<int32_t>(packet[4])
                  | (static_cast<int32_t>(packet[5]) << 8)
                  | (static_cast<int32_t>(packet[6]) << 16);
    if (value & 0x00800000L) value |= 0xFF000000L;
    return value;
}
```

The active firmware profile must define whether the value is an absolute bounded parameter, an accumulated detent count, or another quantity.

## Implemented snapshot sequence

For `N` parameters:

```text
SNAPSHOT_BEGIN, INDEX=N
PARAM_STATE, INDEX=0
...
PARAM_STATE, INDEX=N-1
SNAPSHOT_END, INDEX=N
```

The current implementation serializes current parameter values as events are enqueued. It does not yet include button state.

## RESYNC

`MSG_CMD_RESYNC` is intended as the recovery mechanism after reset, CRC failure, lost packet, partial snapshot, or host reconnect.

Current nested implementation:

1. clears the software TX queue;
2. clears the internal event queue;
3. clears the snapshot-in-progress flag;
4. queues a new framed parameter snapshot.

Important caveat: bytes already loaded into the PIO TX FIFO are not removed by `clearTxQueue()`. A robust master should discard input until it sees a valid fresh `SNAPSHOT_BEGIN` after sending RESYNC.

## IRQ semantics

Design intent: IRQ remains asserted while at least one complete packet remains to be physically transferred.

Current code: IRQ is tied to the software TX queue count and can deassert after the final packet is copied into the PIO FIFO, before the master has clocked all bits. This is a known correctness gap documented in [Known Limitations](KNOWN_LIMITATIONS.md).

## Coherent six-encoder/six-button design target

The full proposed response is:

```text
SNAPSHOT_BEGIN
PARAM_STATE EN1
PARAM_STATE EN2
PARAM_STATE EN3
PARAM_STATE EN4
PARAM_STATE EN5
PARAM_STATE EN6
BUTTON_STATE
SNAPSHOT_END
```

The normative proposal, test vectors, decoder, and reference master are in [`protocol/snapshot-v1.1/`](protocol/snapshot-v1.1/). Do not claim this behavior is implemented until the firmware captures an immutable control sample and emits the button packet.
