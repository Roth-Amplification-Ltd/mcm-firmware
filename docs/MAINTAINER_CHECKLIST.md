# Maintainer Checklist

## Changing a GPIO

- [ ] Update KiCad schematic and PCB.
- [ ] Update `HardwareConfig.h` or `ControlMap.h`.
- [ ] Update PIO absolute `wait gpio` instructions if SCK/CS changed.
- [ ] Update `sm_config_set_jmp_pin()` if CS changed.
- [ ] Update encoder A/B canonicalization table if control pins changed.
- [ ] Update `docs/HARDWARE.md`.
- [ ] Update board diagram if physical numbering changed.
- [ ] Capture and attach logic-analyzer proof.

## Changing the packet envelope

- [ ] Increment protocol version.
- [ ] Define old-version behavior.
- [ ] Update firmware encoder/decoder helpers.
- [ ] Update master reference code.
- [ ] Update test vectors.
- [ ] Add an ADR.
- [ ] Document migration.

## Adding a message type

- [ ] Assign a never-before-used ID.
- [ ] Define direction.
- [ ] Define every byte and reserved-bit rule.
- [ ] Define invalid index behavior.
- [ ] Define whether command changes state.
- [ ] Define response/IRQ behavior.
- [ ] Add CRC test vectors.
- [ ] Add parser tests.

## Changing snapshot behavior

- [ ] Prove snapshots cannot interleave.
- [ ] Prove queue capacity for the whole sequence.
- [ ] Define atomic capture point.
- [ ] Define what happens to concurrent local changes.
- [ ] Define timeout and RESYNC behavior.
- [ ] Verify IRQ remains valid through final physical transfer.

## Before release

- [ ] Clean compile from a fresh checkout.
- [ ] Record Arduino-Pico version and FQBN.
- [ ] Review compiler warnings.
- [ ] Run protocol tests.
- [ ] Run hardware control tests.
- [ ] Verify all six encoder directions.
- [ ] Verify all six button states.
- [ ] Verify reset and RESYNC.
- [ ] Capture SPI logic analyzer trace.
- [ ] Update docs and changelog.
