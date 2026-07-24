# MCM Source Tree Notes

The current repository contains two partially overlapping source layouts.

## Newer hardware-aligned transport slice

Files directly under `src/` contain the corrected KiCad-derived SPI mapping and corrected PIO pin configuration:

- `HardwareConfig.h`
- `SpiSlaveM0.pio`
- `TransportSPI.h/.cpp`
- `Modular_Control_Module_PIO_SPI.ino`

The `.ino` is not build-complete on the current `main` branch because its queue, protocol, parameter, dispatcher, and publisher headers were moved into the nested historical folder.

## Historical integrated protocol tree

`Modular_Control_Module_PIO_SPI_LAYER_A_RX/` contains the more complete dispatcher, event, publisher, snapshot-flow, encoder, button, and RESYNC modules. It also contains an older SPI transport copy and two `.ino` files, so it should be treated as an implementation reference until consolidated.

Read [`../docs/CURRENT_STATUS.md`](../docs/CURRENT_STATUS.md) before building or moving files.
