# MCM SPI Wire Protocol

## Electrical transaction

- Logic level: 3.3 V
- SPI mode: 0
- Bit order: MSB first
- `CS`: active low
- Frame size: exactly 8 bytes
- `SLAVE_IRQ`: active-high level indicating unconsumed response data

The master should clock one eight-byte transaction per asserted `CS` frame.
MOSI always carries a command or a valid NOP packet; MISO simultaneously carries
the next queued response packet.

## Packet envelope

| Byte | Name | Definition |
|---:|---|---|
| 0 | `SYNC` | `0xA5` |
| 1 | `VERSION` | `0x01` |
| 2 | `TYPE` | message/command identifier |
| 3 | `INDEX` | zero-based control index or type-specific count |
| 4 | `DATA0` | least-significant payload byte |
| 5 | `DATA1` | payload byte |
| 6 | `DATA2` | most-significant payload byte |
| 7 | `CRC8` | CRC over bytes 0–6 |

CRC parameters:

```text
polynomial: 0x07
initial value: 0x00
reflection: none
final XOR: 0x00
```

## Message IDs

| ID | Direction | Name | Meaning |
|---:|---|---|---|
| `0x01` | MCM → master | `ENCODER_STATE` | Signed 24-bit absolute encoder count |
| `0x02` | MCM → master | `BUTTON_STATE` | Pressed and long-held bitmaps |
| `0x10` | MCM → master | `SNAPSHOT_BEGIN` | Start of coherent baseline |
| `0x11` | MCM → master | `SNAPSHOT_END` | End of coherent baseline |
| `0x80` | master → MCM | `NOP` | No state change |
| `0x81` | master → MCM | `GET_SNAPSHOT` | Request coherent baseline |
| `0x82` | master → MCM | `RESET_CONTROL` | Reset indexed encoder count to zero |
| `0x83` | master → MCM | `RESET_ALL` | Reset all counts and publish snapshot |
| `0x84` | master → MCM | `SET_LED` | Reserved/unsupported |
| `0x85` | master → MCM | `GET_INFO` | Reserved/unsupported |
| `0x86` | master → MCM | `RESYNC` | Discard stale output and publish fresh baseline |

## Encoder state

```text
TYPE  = 0x01
INDEX = 0..5
DATA0..2 = signed little-endian 24-bit count
```

Valid range is `-8,388,608` through `8,388,607`. Values are clamped before
serialization.

## Button state

```text
TYPE  = 0x02
INDEX = 6
DATA0 = current pressed bitmap
DATA1 = current long-held bitmap
DATA2 = valid button count (6)
```

Bit 0 maps to EN1 and bit 5 maps to EN6. Bits 6 and 7 are zero.

## Snapshot boundaries

```text
TYPE  = 0x10 or 0x11
INDEX = 6
DATA0 = snapshot sequence byte
DATA1 = 0
DATA2 = 0
```

The sequence wraps modulo 256. It is for matching BEGIN and END, not for
security.

## Snapshot order

```text
SNAPSHOT_BEGIN
ENCODER_STATE index 0
ENCODER_STATE index 1
ENCODER_STATE index 2
ENCODER_STATE index 3
ENCODER_STATE index 4
ENCODER_STATE index 5
BUTTON_STATE
SNAPSHOT_END
```

The MCM captures all values before emitting BEGIN. Changes occurring during
serialization are emitted afterward as incremental absolute states.

## Invalid input

Bad sync, unsupported version, or bad CRC packets increment diagnostics and
have no state-changing effect. Unknown or reserved commands are counted and
ignored. A frame containing other than eight bytes is rejected by the transport.
