# Build and Flash Guide

## Toolchain

The firmware targets RP2040 using the Earle Philhower Arduino-Pico core.

Official project references:

- Arduino-Pico installation: <https://arduino-pico.readthedocs.io/en/latest/install.html>
- Arduino-Pico documentation: <https://arduino-pico.readthedocs.io/en/latest/>
- Arduino-Pico source: <https://github.com/earlephilhower/arduino-pico>

## Install with Arduino CLI

```bash
arduino-cli config add board_manager.additional_urls \
  https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
arduino-cli core update-index
arduino-cli core install rp2040:rp2040
```

List candidate board identifiers:

```bash
arduino-cli board listall | grep -i rp2040
```

The final board profile must match the actual MCM flash and USB configuration. Do not blindly select Raspberry Pi Pico if the custom RP2040 board requires a different flash definition.

## Current repository blocker

Arduino treats every `.ino` in one sketch directory as part of the same sketch. The nested Layer-A directory contains two `.ino` files with their own `setup()` and `loop()`, so it cannot be used unchanged as a normal sketch folder.

The newer root sketch has only one `.ino`, but several included support headers were moved into the nested directory. It is therefore also not build-complete as checked in.

Before claiming a successful build, perform the consolidation described in [Implementation Roadmap](IMPLEMENTATION_ROADMAP.md).

## Recommended canonical sketch preparation

1. Create one sketch directory.
2. Keep exactly one `.ino` entry point.
3. Copy the corrected root `HardwareConfig.h`, `TransportSPI.*`, and `SpiSlaveM0.pio` into it.
4. Copy the required protocol, dispatcher, queue, parameter, publisher, and snapshot-flow modules from the nested tree.
5. Remove or rename the legacy minimal `.ino` so Arduino does not concatenate it.
6. Confirm `.pio.h` generation for both PIO sources.
7. Compile with warnings enabled.

## Compile example

After consolidation, a command resembles:

```bash
arduino-cli compile \
  --fqbn rp2040:rp2040:generic \
  /path/to/McmFirmware
```

The exact FQBN and board options are project decisions that must match the custom board. Capture the final command in this document once the hardware profile is frozen.

## First upload

RP2040 can enter its ROM bootloader by holding BOOTSEL while connecting USB. Arduino-Pico also supports subsequent automatic reset/upload when the running firmware and selected upload method support it.

## Serial diagnostics

The sketches currently start `Serial` at 115200. USB CDC often ignores the numerical baud rate electrically, but using 115200 consistently keeps terminal configuration and future UART debugging unambiguous.

## Build acceptance criteria

A build is not considered validated until:

- all local includes resolve;
- PIO headers are generated from the committed `.pio` files;
- no duplicate `setup()` or `loop()` exists;
- warnings are reviewed;
- the produced firmware boots on the target board;
- SPI and IRQ behavior are confirmed with a logic analyzer.
