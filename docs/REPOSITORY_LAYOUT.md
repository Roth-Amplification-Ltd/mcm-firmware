# Repository Layout

## Active firmware

Directory: `src/MCM_Firmware/`

| File | Responsibility |
|---|---|
| `MCM_Firmware.ino` | Minimal Arduino entry point |
| `McmApplication.h/.cpp` | Cooperative superloop and application state machine |
| `McmTypes.h` | Fixed-width project types and compile-time assumptions |
| `HardwareConfig.h` | Electrical constants, queue sizes, timing, and SPI resources |
| `ControlMap.h` | EN1–EN6 GPIO and PIO assignments |
| `ControlState.h` | Authoritative state, snapshots, and change records |
| `ControlScanner.h/.cpp` | Six-encoder and six-button sampling |
| `EncoderPIO.h/.cpp` | Bounded quadrature decoding and count management |
| `EncoderPIO.pio` | Encoder phase-change PIO program |
| `DebouncedButton.h/.cpp` | Button debounce and long-hold state machine |
| `CommandDispatcher.h/.cpp` | Packet validation and typed command dispatch |
| `SnapshotPublisher.h/.cpp` | Incremental snapshot serialization state machine |
| `EventQueue.h` | Fixed-capacity allocation-free ring buffer |
| `TransportProtocol.h` | Packet format, CRC, constructors, and parser |
| `TransportSPI.h/.cpp` | PIO-backed SPI transport and level IRQ |
| `SpiSlaveM0.pio` | SPI Mode 0 PIO program |
| `Diagnostics.h` | Saturating transport and recovery counters |

Only one `.ino` file exists in the active sketch directory, and all local
includes required by that sketch are present in the same directory.

## Verification

| Path | Purpose |
|---|---|
| `tests/host/` | Host tests for protocol, queues, commands, buttons, and snapshots |
| `tools/check_misra_like.py` | Enforced native-source engineering profile |
| `tools/check_license_headers.py` | MPL/SPDX source-header verification |
| `.github/workflows/misra-like-quality.yml` | Continuous integration checks |

## Documentation

`docs/` contains the engineering handbook, protocol reference material,
architectural decisions, diagrams, and hardware-derived control maps.

## Legacy reference source

`archive/` contains superseded experiments retained for comparison and
attribution. Nothing under `archive/` is part of the active firmware build or
native-code quality scope.
