# MCM Deterministic/MISRA-Like Validation Report

## Passed in the generated package

- Native profile checker passed for all canonical source files.
- Host build used C++17 with:
  - `-Wall`
  - `-Wextra`
  - `-Wpedantic`
  - `-Wconversion`
  - `-Wsign-conversion`
  - `-Wshadow`
  - `-Werror`
- All canonical `.cpp` modules compiled together against narrow RP2040/Arduino
  host stubs.
- The Arduino `.ino` entry point compiled as a C++ translation unit.
- CRC and packet validation tests passed.
- Signed 24-bit encode/decode tests passed.
- Fixed-capacity queue overflow/order tests passed.
- Typed command-dispatch tests passed.
- Explicit button debounce/long-hold state-machine tests passed.
- Complete nine-packet immutable snapshot ordering tests passed.
- ZIP integrity and SHA-256 manifest verification passed.

## Not validated in this environment

- Arduino-Pico target compilation for the real RP2040 board definition.
- `pioasm` assembly/generation of the two PIO sources.
- Flashing and execution on an MCM PCB.
- Encoder electrical direction and detent count on the physical controls.
- Button timing against real switch bounce.
- Maximum supported SPI clock.
- PIO FIFO pressure and TX-underrun recovery on hardware.
- Exact IRQ deassertion observed by a logic analyzer.
- Worst-case loop execution time, stack high-water use, watchdog, and brownout
  recovery.
- Formal MISRA C++:2023 analysis or compliance evidence.

## Required board-validation sequence

```bash
python3 tools/check_license_headers.py
python3 tools/check_misra_like.py
bash tests/host/run.sh
```

Then compile `src/MCM_Firmware/MCM_Firmware.ino` with the pinned Arduino-Pico
core, flash one board, and capture IRQ/SCK/CS/MISO/MOSI while running startup,
GET_SNAPSHOT, encoder movement, button press/hold/release, RESET, and RESYNC.
