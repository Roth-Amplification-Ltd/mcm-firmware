# Contributing to MCM Firmware

MCM firmware changes affect a physical control surface and an external master-controller protocol. Treat pin mappings, packet layouts, and message semantics as hardware interfaces rather than ordinary implementation details.

## Before changing code

Read:

1. [`docs/CURRENT_STATUS.md`](docs/CURRENT_STATUS.md)
2. [`docs/HARDWARE.md`](docs/HARDWARE.md)
3. [`docs/SPI_PROTOCOL.md`](docs/SPI_PROTOCOL.md)
4. [`docs/MAINTAINER_CHECKLIST.md`](docs/MAINTAINER_CHECKLIST.md)

## Required discipline

- Do not introduce I²C host-interface code unless the hardware is revised and the decision is recorded in a new ADR.
- Do not change GPIO assignments in only one file. The KiCad project, `HardwareConfig.h`, PIO program, source comments, and hardware documentation must agree.
- Do not change the eight-byte packet envelope without incrementing the protocol version and documenting migration behavior.
- Do not reuse a message ID with new semantics.
- Do not serialize compiler structs directly onto the wire.
- Do not silently drop queue-overflow errors. Add a counter or status flag and document it.
- Keep functional changes separate from comment-only documentation changes whenever practical.

## Commit scope

Prefer focused commits such as:

```text
Document SPI transaction timing
Wire EN1–EN6 into parameter table
Add button-state snapshot packet
Fix IRQ deassertion after physical transfer
```

A pull request should state:

- what changed;
- why it changed;
- whether the wire protocol changed;
- whether the KiCad mapping changed;
- which tests or logic-analyzer captures were used;
- known limitations that remain.

## Source comments

Comments should explain invariants, ownership, timing assumptions, and non-obvious RP2040 PIO behavior. Avoid comments that merely restate a line of code.

Good:

```cpp
// The queue entry is retired only after all eight bytes are handed to the PIO
// FIFO. This does not prove that the master has physically clocked the packet.
```

Weak:

```cpp
// Increment index.
index++;
```

## Testing expectations

Protocol changes require byte-level test vectors. Transport changes require at least one logic-analyzer capture showing CS, SCK, MOSI, MISO, and IRQ. Encoder changes require clockwise/counter-clockwise transition tests and invalid-transition tests. Button changes require debounce and long-press timing tests.
