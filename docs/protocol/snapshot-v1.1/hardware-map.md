# MCM Hardware and Control Numbering

The mapping below is taken from the current `Modular-Control-Module.kicad_sch` and `Modular-Control-Module.kicad_pcb` design.

## External SPI host interface

| Signal | RP2040 GPIO | Direction | Electrical behavior |
|---|---:|---|---|
| `SLAVE_IRQ` | 11 | MCM → master | Active high, 3.3 V |
| `SPI_SCK` | 12 | Master → MCM | Mode 0 clock, idle low |
| `SPI_CS` | 13 | Master → MCM | Active low; one 8-byte packet per assertion |
| `SPI_MISO` | 14 | MCM → master | MSB-first data |
| `SPI_MOSI` | 15 | Master → MCM | MSB-first data |

There are no external host-interface SDA or SCL nets in the released KiCad project.

## Encoder and push-button indexes

Packet indexes are zero-based. KiCad references and front-panel labels are one-based.

| Packet index | KiCad reference | Physical position, component side | Encoder A | Encoder B | Push-button |
|---:|---|---|---:|---:|---:|
| `0` | `EN1` | Top left | GPIO4 | GPIO5 | GPIO3 |
| `1` | `EN2` | Top center | GPIO7 | GPIO6 | GPIO8 |
| `2` | `EN3` | Top right | GPIO19 | GPIO20 | GPIO18 |
| `3` | `EN4` | Bottom left | GPIO1 | GPIO2 | GPIO0 |
| `4` | `EN5` | Bottom center | GPIO26 | GPIO27 | GPIO28 |
| `5` | `EN6` | Bottom right | GPIO25 | GPIO24 | GPIO23 |

The corresponding button bitmap uses the same index in the bit position:

```text
bit 0 = EN1 push-button
bit 1 = EN2 push-button
bit 2 = EN3 push-button
bit 3 = EN4 push-button
bit 4 = EN5 push-button
bit 5 = EN6 push-button
bits 6–7 = reserved and transmitted as zero
```

Button electrical polarity is normalized by firmware: a bitmap bit of `1` always means **pressed**, even though the physical switch input may be active low.
