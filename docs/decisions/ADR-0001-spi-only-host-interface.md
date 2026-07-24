# ADR-0001: SPI-Only External Host Interface

**Status:** Accepted

## Context

Early firmware architecture mentioned both SPI and I²C. The released KiCad project exposes `SPI_SCK`, `SPI_CS`, `SPI_MISO`, `SPI_MOSI`, and `SLAVE_IRQ`; it does not route external SDA/SCL host nets.

## Decision

The current hardware and firmware use SPI as the sole external host-control transport.

## Consequences

- Firmware must not advertise I²C host support.
- GPIO11–15 are reserved for host communication.
- Future I²C support requires a hardware revision and a new ADR.
