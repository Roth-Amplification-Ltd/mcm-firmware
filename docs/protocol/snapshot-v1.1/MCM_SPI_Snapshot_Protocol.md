# MCM SPI Coherent Snapshot Protocol

**Specification revision:** 1.1  
**Wire protocol version byte:** `0x01`  
**Transport:** SPI Mode 0, MSB-first, 3.3 V  
**Frame size:** exactly 8 bytes per chip-select assertion

## 1. Purpose

This extension defines how a master controller obtains one coherent sample of:

- all six rotary encoder values; and
- the current state of all six encoder push-buttons.

It preserves the existing 8-byte MCM packet envelope and existing 24-bit parameter values. A snapshot is a **framed packet sequence**, not one oversized SPI frame.

## 2. Transport contract

- `SPI_CS` is active low.
- The master clocks exactly 64 SCK pulses while `SPI_CS` is low.
- Every CS assertion therefore transfers one 8-byte packet.
- SPI mode is CPOL=0, CPHA=0.
- Bytes are shifted MSB-first.
- The MCM validates a command only after the complete CS-framed packet is received.
- The command frame itself does not contain the newly requested response. The response is queued afterward.
- `SLAVE_IRQ` is asserted high when one or more complete response packets are ready to be clocked by the master.
- `SLAVE_IRQ` remains high until the final queued response packet has actually been transferred.

The master should send eight `0x00` bytes while reading each queued response packet. Those dummy MOSI bytes do not form a valid command and must be ignored by the MCM command dispatcher.

## 3. Common 8-byte packet envelope

| Byte | Field | Description |
|---:|---|---|
| 0 | `SYNC` | Always `0xA5` |
| 1 | `VERSION` | Currently `0x01` |
| 2 | `TYPE` | Command or response message ID |
| 3 | `INDEX` | Control index, bitmap bank, or control count, depending on type |
| 4 | `DATA0` | Type-specific payload byte |
| 5 | `DATA1` | Type-specific payload byte |
| 6 | `DATA2` | Type-specific payload byte |
| 7 | `CRC8` | CRC-8 over bytes 0 through 6 |

Do not serialize a compiler struct directly. Construct and decode the eight explicit bytes so padding and host endianness cannot alter the wire format.

## 4. CRC-8

| Property | Value |
|---|---|
| Polynomial | `0x07` (`x^8 + x^2 + x + 1`) |
| Initial value | `0x00` |
| Input reflection | No |
| Output reflection | No |
| Final XOR | `0x00` |
| Covered bytes | Packet bytes 0–6 |

A packet with an invalid sync byte, unsupported protocol version, or invalid CRC must be discarded without changing control state.

## 5. Message IDs

### MCM to master

| Name | ID | Purpose |
|---|---:|---|
| `MSG_PARAM_STATE` | `0x01` | One encoder/parameter value |
| `MSG_BUTTON_STATE` | `0x02` | Current push-button bitmaps |
| `MSG_SNAPSHOT_BEGIN` | `0x10` | Opens a coherent snapshot |
| `MSG_SNAPSHOT_END` | `0x11` | Closes and validates a coherent snapshot |

`MSG_BUTTON_STATE` replaces the unused early draft name `MSG_BUTTON_EVENT`. It represents sampled state, not a momentary event packet.

### Master to MCM

| Name | ID | Purpose |
|---|---:|---|
| `MSG_CMD_NOP` | `0x80` | Bus test; no state change |
| `MSG_CMD_GET_SNAPSHOT` | `0x81` | Request all six encoder and button states |
| `MSG_CMD_RESET_PARAM` | `0x82` | Existing single-parameter reset command |
| `MSG_CMD_RESET_ALL` | `0x83` | Existing all-parameter reset command |

## 6. Snapshot request

The master sends:

| Byte | Value |
|---:|---:|
| 0 | `0xA5` |
| 1 | `0x01` |
| 2 | `0x81` |
| 3–6 | `0x00` |
| 7 | CRC-8 |

Canonical request:

```text
A5 01 81 00 00 00 00 5D
```

On receipt, the MCM must capture the six encoder values and button state into a temporary snapshot buffer before queuing the first response packet. Local control scanning may continue afterward, but it must not alter the already captured snapshot sequence.

## 7. Snapshot response sequence

A complete response contains exactly nine packets in this order:

```text
MSG_SNAPSHOT_BEGIN
MSG_PARAM_STATE index 0  (EN1)
MSG_PARAM_STATE index 1  (EN2)
MSG_PARAM_STATE index 2  (EN3)
MSG_PARAM_STATE index 3  (EN4)
MSG_PARAM_STATE index 4  (EN5)
MSG_PARAM_STATE index 5  (EN6)
MSG_BUTTON_STATE bank 0
MSG_SNAPSHOT_END
```

A master must reject the snapshot when:

- begin or end framing is missing;
- begin and end sequence IDs differ;
- any encoder index is missing or duplicated;
- the button packet is missing;
- any packet fails CRC or version validation; or
- the count in the begin/end packet is not six.

The master retains its previous valid state if the new snapshot is rejected.

## 8. Snapshot boundary packets

Both `MSG_SNAPSHOT_BEGIN` and `MSG_SNAPSHOT_END` use:

| Byte | Meaning |
|---:|---|
| 2 | `0x10` for begin or `0x11` for end |
| 3 | Control count; currently `6` |
| 4 | Snapshot sequence ID, low byte |
| 5 | Snapshot sequence ID, high byte |
| 6 | Snapshot status flags |

The 16-bit sequence ID increments once per captured snapshot and wraps naturally from `0xFFFF` to `0x0000`.

### Snapshot status flags, byte 6

| Bit | Name | Meaning |
|---:|---|---|
| 0 | `EVENT_QUEUE_OVERFLOW` | A local event or response queue overflow occurred since the prior valid snapshot |
| 1 | `CONTROL_SCAN_OVERRUN` | Firmware missed its intended control-scan deadline |
| 2–7 | Reserved | Transmit as zero |

The begin and end packets must repeat the same sequence ID, count, and status byte.

## 9. Encoder value packet

`MSG_PARAM_STATE` retains the existing signed 24-bit value representation:

| Byte | Meaning |
|---:|---|
| 2 | `0x01` |
| 3 | Encoder index `0–5` |
| 4 | Value bits 7–0 |
| 5 | Value bits 15–8 |
| 6 | Value bits 23–16 |

Decode as little-endian two's-complement signed 24-bit:

```text
raw = DATA0 | DATA1 << 8 | DATA2 << 16
if raw bit 23 is set, sign-extend bits 31–24
```

Valid representable range:

```text
-8,388,608 through +8,388,607
```

The application may use this as an absolute parameter value, bounded encoder position, or accumulated detent count. That policy is outside the transport specification, but must remain consistent for a given firmware profile.

## 10. Button state packet

`MSG_BUTTON_STATE` is a compact snapshot of all six encoder push-buttons:

| Byte | Meaning |
|---:|---|
| 2 | `0x02` |
| 3 | Bitmap bank; currently `0` |
| 4 | Debounced pressed-state bitmap |
| 5 | Long-held-state bitmap |
| 6 | Number of valid button bits; currently `6` |

### Bitmap mapping

| Bit | Button |
|---:|---|
| 0 | EN1 button |
| 1 | EN2 button |
| 2 | EN3 button |
| 3 | EN4 button |
| 4 | EN5 button |
| 5 | EN6 button |
| 6–7 | Reserved, zero |

A `1` in byte 4 means the button is currently pressed after debounce. A `1` in byte 5 means the same button is pressed and has met the firmware's configured long-hold threshold. The long-held bitmap may be all zero when long-hold classification is disabled.

Press and release edges are intentionally not included in a coherent state snapshot. A polling master detects edges by comparing accepted snapshots. Separate asynchronous event messages may be defined later without changing this snapshot format.

## 11. Master transaction

Recommended sequence:

1. Ensure `SPI_CS` is high.
2. Send one valid `MSG_CMD_GET_SNAPSHOT` packet.
3. Wait for `SLAVE_IRQ` to become high, with a timeout.
4. While IRQ is high, clock one 8-byte dummy frame and validate the returned packet.
5. Assemble packets beginning with `SNAPSHOT_BEGIN`.
6. Stop after the matching `SNAPSHOT_END` is received.
7. Validate completeness and atomically publish the new state to the host application.

The master should impose both a time limit and a maximum packet count so corrupted firmware cannot hold the transaction open indefinitely. For the current six-control format, receiving more than nine response packets before a matching end packet is an error.

## 12. Canonical example

Example snapshot sequence ID `0x1234`:

- EN1 = `0`, released
- EN2 = `12`, pressed
- EN3 = `-3`, released
- EN4 = `127`, pressed and long-held
- EN5 = `-128`, released
- EN6 = `1024`, pressed

```text
GET_SNAPSHOT      A5 01 81 00 00 00 00 5D
SNAPSHOT_BEGIN    A5 01 10 06 34 12 00 D9
PARAM_STATE EN1   A5 01 01 00 00 00 00 CA
PARAM_STATE EN2   A5 01 01 01 0C 00 00 26
PARAM_STATE EN3   A5 01 01 02 FD FF FF 3F
PARAM_STATE EN4   A5 01 01 03 7F 00 00 D0
PARAM_STATE EN5   A5 01 01 04 80 FF FF BD
PARAM_STATE EN6   A5 01 01 05 00 04 00 D0
BUTTON_STATE      A5 01 02 00 2A 08 06 12
SNAPSHOT_END      A5 01 11 06 34 12 00 BB
```

Pressed bitmap `0x2A` sets bits 1, 3, and 5: EN2, EN4, and EN6. Long-held bitmap `0x08` sets bit 3: EN4.

## 13. Firmware implementation requirements

- Debounce all six button inputs before capturing byte 4.
- Normalize physical active-low inputs so `1` means pressed on the wire.
- Capture all encoder values and both button bitmaps into one immutable temporary snapshot.
- Queue the complete nine-packet sequence without interleaving unrelated asynchronous state packets.
- Keep IRQ asserted until the final packet has been clocked out, not merely copied into the RP2040 PIO TX FIFO.
- If the response queue cannot hold all nine packets, do not emit a partial snapshot. Set an overflow diagnostic for the next successful snapshot.
- Reserved bits and bytes must be transmitted as zero.
