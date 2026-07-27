# Build and Flash

The repository has one supported Arduino sketch:

```text
src/MCM_Firmware/MCM_Firmware.ino
```

Both Arduino IDE and the terminal build use the same source directory and the
same committed PIO-generated headers.

## Hardware target

The MCM board uses:

- RP2040;
- W25Q16JV 16-Mbit / 2-Mbyte QSPI flash;
- 133 MHz system clock;
- no flash filesystem;
- Pico SDK USB stack;
- Arduino-Pico Generic RP2040 board definition;
- W25Q16JVxQ QSPI `/4` boot stage.

## Terminal build

Install the project-local toolchain once:

```bash
./setup.sh
```

Then build:

```bash
./build.sh
```

The UF2 is written to:

```text
build/arduino/mcm-firmware.uf2
```

Equivalent Make targets are available:

```bash
make setup
make build
make test
make upload PORT=/dev/ttyACM0
```

The setup script installs Arduino CLI and Arduino-Pico into `.tools/` inside the
checkout, so it does not depend on or modify a global Arduino installation.

The default build uses Arduino CLI 1.5.1 and Arduino-Pico 5.6.0. Override either
version when needed:

```bash
ARDUINO_CLI_VERSION=1.5.1 MCM_CORE_VERSION=5.6.0 ./setup.sh
```

Override the complete board selection for experiments:

```bash
MCM_FQBN='rp2040:rp2040:generic:...' ./build.sh
```

## Arduino IDE

1. Install Arduino IDE 2.x.
2. Add this Boards Manager URL in **File → Preferences**:

   ```text
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```

3. Install **Raspberry Pi Pico/RP2040** by Earle F. Philhower.
4. Open `src/MCM_Firmware/MCM_Firmware.ino`.
5. Select **Generic RP2040**.
6. Select these board options:

   | Option | Selection |
   |---|---|
   | Flash Size | 2 MB, no filesystem |
   | CPU Speed | 133 MHz |
   | Optimize | Small `-Os` |
   | Operating System | None |
   | RTTI | Disabled |
   | Exceptions | Disabled |
   | USB Stack | Pico SDK |
   | Boot Stage 2 | W25Q16JVxQ QSPI `/4` |

7. Click **Verify**.
8. Use **Sketch → Export Compiled Binary** to produce a UF2, or upload over USB.

For the first USB upload, hold BOOTSEL while connecting the board and select the
UF2 boot device. After compatible firmware is running, normal serial-port upload
can be used.

## PIO sources

`EncoderPIO.pio` and `SpiSlaveM0.pio` are the maintained PIO assembly sources.

Their generated headers are committed beside them:

```text
EncoderPIO.pio.h
SpiSlaveM0.pio.h
```

Committing the generated headers is intentional: Arduino IDE does not run a
project-specific pre-build hook for arbitrary sketch PIO files. The committed
headers make a clean checkout directly buildable.

After changing either `.pio` source, regenerate the matching header with the
`pioasm` tool included by Arduino-Pico, review the instruction diff, and run both
the terminal build and hardware tests.

## Upload from the terminal

Build and upload to an existing serial port:

```bash
./upload.sh /dev/ttyACM0
```

For a fresh or unresponsive board, build the UF2 with `./build.sh`, hold BOOTSEL
while connecting USB, and copy `build/arduino/mcm-firmware.uf2` to the mounted
RP2040 boot volume.

## Required release checks

```bash
python3 tools/check_license_headers.py
python3 tools/check_misra_like.py
bash tests/host/run.sh
./build.sh
```

A release record should include the Git commit, Arduino-Pico version, generated
UF2 hash, board revision, logic-analyzer captures, and maximum verified SPI
clock.
