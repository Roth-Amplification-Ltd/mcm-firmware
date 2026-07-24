# Implementation Roadmap

## Phase 1 — Consolidate the source tree

- Choose one canonical sketch directory.
- Keep exactly one `.ino` file.
- Move the corrected root SPI transport and PIO source into that sketch.
- Move protocol, queue, dispatcher, publisher, and snapshot-flow modules beside it.
- Delete or archive duplicate historical source copies.
- Add a successful reproducible `arduino-cli` compile command.

## Phase 2 — Centralize hardware definition

- Extend `HardwareConfig.h` or add `ControlMap.h` with EN1–EN6 A/B/button pins.
- Add compile-time assertions for six controls and eight-byte packets.
- Define PIO block/state-machine allocation in one table.
- Share one encoder PIO program offset per PIO block.

## Phase 3 — Wire the six controls

- Instantiate six encoder decoders.
- Instantiate six button debouncers.
- Poll them in the main loop.
- Convert raw transition counts to detents.
- Bind each encoder to parameter index 0–5.
- Generate events only when authoritative state changes.
- Keep encoder acceleration disabled.

## Phase 4 — Implement coherent snapshots

- Add immutable `ControlSnapshot` capture.
- Add `MSG_BUTTON_STATE`.
- Add sequence ID and status flags.
- Ensure the queue can atomically accept the entire response.
- Prevent unrelated event interleaving.
- Keep IRQ high through physical transfer of END.

## Phase 5 — Harden transport correctness

- Track PIO-consumed versus physically clocked bytes.
- Define idle MISO filler bytes.
- Flush or invalidate stale PIO FIFO data during RESYNC.
- Replace polled CS framing with PIO-generated frame markers or IRQs.
- Add overflow and framing counters.

## Phase 6 — Tests

- Host-buildable CRC and packet unit tests.
- Snapshot parser tests.
- Event and TX queue boundary tests.
- Quadrature transition table tests.
- Button debounce timing tests.
- Hardware-in-loop master/MCM transaction tests.
- Logic-analyzer golden captures.

## Phase 7 — Release process

- Tag protocol and firmware versions together.
- Generate a firmware binary and checksum.
- Publish build toolchain/version information.
- Record hardware revision compatibility.
- Update changelog and ADRs for every interface change.
