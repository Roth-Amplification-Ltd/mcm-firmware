# Maintainer Checklist

## Every change

- [ ] Canonical source remains under `src/MCM_Firmware/`.
- [ ] No second `.ino` is added to the canonical sketch folder.
- [ ] MPL/SPDX headers are present.
- [ ] `python3 tools/check_misra_like.py` passes.
- [ ] `bash tests/host/run.sh` passes.
- [ ] Every queue/result failure is handled.
- [ ] No dynamic allocation or Arduino `String` is introduced.
- [ ] New persistent behavior has an explicit state and legal transitions.
- [ ] Loops over external data have an explicit bound.

## Pin or PIO changes

- [ ] KiCad, `ControlMap.h`, `HardwareConfig.h`, PIO comments, and docs agree.
- [ ] PIO state machines do not collide.
- [ ] Encoder A/B pins remain adjacent or the implementation is redesigned.
- [ ] Board-level direction and button tests pass.

## Protocol changes

- [ ] Message ID is unique.
- [ ] Versioning/backward compatibility is decided.
- [ ] Reserved fields and byte order are explicit.
- [ ] Host tests include canonical and invalid packets.
- [ ] Snapshot ordering and atomicity remain intact.
- [ ] Master integration docs are updated.

## Transport changes

- [ ] Logic-analyzer captures include IRQ, CS, SCK, MOSI, and MISO.
- [ ] Short/long/malformed frames are tested.
- [ ] IRQ assertion and physical frame completion are verified.
- [ ] Maximum tested SPI clock is recorded.

## Compliance claim changes

- [ ] “MISRA-like” is not changed to “MISRA compliant” without formal evidence.
- [ ] New adopted-code use is recorded in `MISRA_DEVIATIONS.md`.
- [ ] Analyzer deviations identify risk, containment, evidence, reviewer, and date.
