# Repository Layout

## Canonical firmware

Directory: `src/MCM_Firmware/`

| File | Responsibility |
|---|---|
| `MCM_Firmware.ino` | Minimal Arduino entry point; delegates to `McmApplication` |
| `McmApplication.h/.cpp` | Cooperative system superloop and explicit application state machine |
| `McmTypes.h` | Fixed-width project types and common compile-time assumptions |
| `HardwareConfig.h` | Electrical constants, queue sizes, timing constants, and SPI resources |
| `ControlMap.h` | EN1–EN6 GPIO and PIO state-machine assignments |
| `ControlState.h` | Immutable snapshot and change-set records |
| `ControlScanner.h/.cpp` | Six encoder and six button sampling; authoritative state owner |
| `EncoderPIO.h/.cpp` | Bounded quadrature decoder service and saturating count |
| `EncoderPIO.pio` | Encoder phase-change stream state machine |
| `DebouncedButton.h/.cpp` | Explicit active-low debounce/long-hold state machine |
| `CommandDispatcher.h/.cpp` | Validates packets and converts commands into typed actions |
| `SnapshotPublisher.h/.cpp` | Snapshot serialization FSM and incremental publication queue |
| `EventQueue.h` | Fixed-capacity allocation-free ring buffer |
| `TransportProtocol.h` | Eight-byte wire format, CRC, typed packet constructors and parser |
| `TransportSPI.h/.cpp` | PIO-backed SPI peripheral, framed RX, bounded TX queue, level IRQ |
| `SpiSlaveM0.pio` | SPI Mode 0 bit engine and frame-end marker generation |
| `Diagnostics.h` | Saturating fault and recovery counters |

## Archived source

Directory: `archive/pre-misra-canonicalization/`

This directory preserves the previous corrected root transport slice and the
historical Layer-A/Layer-D development sketch. It is retained for archaeology,
comparison, and attribution. It is not compiled as part of the canonical
firmware and is excluded from the native-code MISRA-like scope.

## Verification support

| Path | Purpose |
|---|---|
| `tests/host/` | Native host tests for packet, queue, command, and snapshot logic |
| `tools/check_misra_like.py` | Lightweight enforceable project-profile checker |
| `tools/check_license_headers.py` | MPL/SPDX source-header check |
| `.github/workflows/misra-like-quality.yml` | CI execution of profile and host tests |
| `docs/MISRA_*.md` | Profile, matrix, and deviation evidence |

## Build rule

Only one `.ino` file exists in the canonical sketch directory. Every local
include required by that sketch is present in the same directory, allowing the
Arduino build system to treat it as one complete sketch.
