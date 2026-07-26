# Build and Flash

## Canonical sketch

Open:

```text
src/MCM_Firmware/MCM_Firmware.ino
```

Do not open an archived sketch as the production target.

## Required environment

- RP2040-compatible board/toolchain
- Earle Philhower Arduino-Pico core
- PIO source generation enabled by that core
- board selection matching the MCM RP2040 hardware and flash configuration

## Pre-build checks

```bash
python3 tools/check_license_headers.py
python3 tools/check_misra_like.py
bash tests/host/run.sh
```

## Arduino IDE flow

1. Open `src/MCM_Firmware/MCM_Firmware.ino`.
2. Select the appropriate RP2040 board definition.
3. Select the intended flash size and USB/serial options.
4. Compile before connecting the production board.
5. Hold BOOTSEL while connecting USB when a UF2 boot upload is required.
6. Upload and verify the startup snapshot/IRQ behavior with the master or a test
   fixture.

## Release evidence

A production release should record:

- exact Git commit;
- Arduino-Pico version;
- compiler version and flags;
- generated UF2 SHA-256;
- host test result;
- native profile result;
- target build result;
- logic-analyzer capture set;
- maximum tested SPI clock;
- board hardware revision.

The overlay was host-tested, but an actual Arduino-Pico target build and hardware
run must still be performed in the user's development environment.
