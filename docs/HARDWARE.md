# Hardware and Control Map

This map was verified against the uploaded `Modular-Control-Module.kicad_sch` and `Modular-Control-Module.kicad_pcb` sources.

## Electrical assumptions

- RP2040 and host-interface logic operate at 3.3 V.
- `SPI_CS` is active low.
- `SLAVE_IRQ` is active high.
- Encoder phase inputs and push-buttons use pull-ups and switch to ground.
- The external host interface is SPI-only.
- The RP2040's QSPI flash bus is internal and must not be confused with the host interface.

## Host connector signals

| Net | RP2040 GPIO | Direction from MCM | Function |
|---|---:|---|---|
| `SLAVE_IRQ` | 11 | Output | Level-based indication that response data is pending |
| `SPI_SCK` | 12 | Input | External clock, idle low |
| `SPI_CS` | 13 | Input | Active-low packet boundary |
| `SPI_MISO` | 14 | Output | MCM response/state data |
| `SPI_MOSI` | 15 | Input | Master command data |

## Physical encoder numbering

Viewed from the component side with the USB connector toward the lower edge:

```text
+---------+---------+---------+
| EN1 / 0 | EN2 / 1 | EN3 / 2 |
+---------+---------+---------+
| EN4 / 3 | EN5 / 4 | EN6 / 5 |
+---------+---------+---------+
```

KiCad references are one-based (`EN1`–`EN6`). Protocol indexes are zero-based (`0`–`5`).

![MCM encoder numbering](assets/mcm-board-encoder-numbering.png)

## Encoder and push-button GPIO map

| Protocol index | KiCad ref. | Position | Phase A | Phase B | Push switch |
|---:|---|---|---:|---:|---:|
| 0 | `EN1` | Top left | GPIO4 | GPIO5 | GPIO3 |
| 1 | `EN2` | Top center | GPIO7 | GPIO6 | GPIO8 |
| 2 | `EN3` | Top right | GPIO19 | GPIO20 | GPIO18 |
| 3 | `EN4` | Bottom left | GPIO1 | GPIO2 | GPIO0 |
| 4 | `EN5` | Bottom center | GPIO26 | GPIO27 | GPIO28 |
| 5 | `EN6` | Bottom right | GPIO25 | GPIO24 | GPIO23 |

The phase pairs are physically consecutive GPIO numbers, although EN2 and EN6 are ordered B/A in ascending GPIO order. `EncoderPIO::canonicalize()` exists specifically so caller-supplied A/B meaning can be preserved when the lower-numbered pin is B.

## Button bitmap convention

The design-target snapshot packet uses the same zero-based index as the bit number:

```text
bit 0 = EN1 push-button
bit 1 = EN2 push-button
bit 2 = EN3 push-button
bit 3 = EN4 push-button
bit 4 = EN5 push-button
bit 5 = EN6 push-button
bits 6–7 = reserved and zero
```

Firmware must normalize the active-low electrical input so a wire value of `1` means **pressed**.

## RP2040 PIO resource plan

The current code uses `PIO1`, state machine 2 for the SPI peripheral. A six-encoder PIO implementation requires six additional state machines. RP2040 provides four state machines per PIO block, so the encoder scanners must be distributed across PIO0 and the remaining PIO1 state machines, or redesigned to scan multiple encoders per state machine.

A possible allocation is:

| Function | PIO block | State machine |
|---|---|---:|
| EN1 | PIO0 | 0 |
| EN2 | PIO0 | 1 |
| EN3 | PIO0 | 2 |
| EN4 | PIO0 | 3 |
| EN5 | PIO1 | 0 |
| EN6 | PIO1 | 1 |
| SPI peripheral | PIO1 | 2 |
| Reserved | PIO1 | 3 |

This allocation is a recommendation, not yet implemented by the active sketch.
