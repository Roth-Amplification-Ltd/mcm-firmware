# Repository Layout

## Root

| Path | Description |
|---|---|
| `README.md` | Project overview and documentation entry point |
| `CONTRIBUTING.md` | Change discipline for hardware, protocol, and firmware work |
| `Doxyfile` | Optional Doxygen configuration for source API documentation |
| `LICENSE` | Repository license |
| `docs/` | Engineering handbook, diagrams, decisions, protocol proposal, tests, and tools |

## Newer hardware-aligned source slice

| Path | Description |
|---|---|
| `src/HardwareConfig.h` | KiCad-verified SPI host GPIO constants |
| `src/SpiSlaveM0.pio` | Corrected SPI Mode 0 PIO program using absolute GPIO waits and configured `jmp pin` |
| `src/TransportSPI.h/.cpp` | Newer SPI transport implementation using `HardwareConfig.h` |
| `src/Modular_Control_Module_PIO_SPI.ino` | Newer integration sketch; currently incomplete because its support headers were moved elsewhere |

## Historical integrated Layer-A/Layer-D source tree

Directory: `src/Modular_Control_Module_PIO_SPI_LAYER_A_RX/`

| File | Responsibility |
|---|---|
| `Modular_Control_Module_PIO_SPI_LAYER_A_RX.ino` | SPI command/response loop, RESYNC handling, placeholder parameter model |
| `Modular_Control_Module.ino` | Older minimal publisher demonstration; conflicts with the other `.ino` when compiled in the same Arduino sketch folder |
| `TransportProtocol.h` | Packet constants, message IDs, CRC-8 |
| `CommandDispatcher.h/.cpp` | Validates commands, mutates parameter state, enqueues snapshot/reset events |
| `EventQueue.h` | Fixed-capacity single-threaded ring buffer |
| `StatePublisher.h/.cpp` | Converts internal events into eight-byte packets |
| `SnapshotFlow.h` | Shared guard preventing overlapping snapshots |
| `ParamBinding.h` | Minimal authoritative parameter record |
| `TransportSPI.h/.cpp` | Older SPI transport copy with RESYNC queue clearing |
| `SpiSlaveM0.pio` | Older SPI PIO program; not the corrected root version |
| `EncoderPIO.h/.cpp` | Quadrature state decoder driven by an encoder PIO state stream |
| `EncoderPIO.pio` | Pushes a sample when either encoder phase changes |
| `DebouncedButton.h` | Active-low button debounce and long-press edge detection |

## Recommended future layout

The production tree should eventually resemble:

```text
src/
  McmFirmware.ino
  HardwareConfig.h
  ControlMap.h
  EncoderScanner.h/.cpp
  ButtonScanner.h/.cpp
  EventQueue.h
  ParamBinding.h
  CommandDispatcher.h/.cpp
  StatePublisher.h/.cpp
  SnapshotFlow.h
  TransportProtocol.h
  TransportSPI.h/.cpp
  SpiSlaveM0.pio
  EncoderPIO.pio
```

Only one `.ino` file should exist in the sketch folder, and every local include should resolve from that same folder or a conventional library directory.
