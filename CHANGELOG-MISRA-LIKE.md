# MCM Deterministic/MISRA-Like Hardening Update

## Structural changes

- Added one canonical Arduino sketch directory: `src/MCM_Firmware/`.
- Added a safe migration script that moves the superseded source trees into
  `archive/pre-misra-canonicalization/` without deleting history.
- Reduced the Arduino entry point to delegation through `McmApplication`.

## Deterministic runtime changes

- Added explicit application states: Starting, Running, Resynchronizing, Fault.
- Added explicit six-state button debounce/long-hold state machines.
- Added explicit snapshot publication phases.
- Added explicit SPI RX frame state, PIO frame-end markers, and TX-underrun recovery.
- Bounded encoder FIFO draining and normal per-loop publication work.

## Control implementation

- Instantiated EN1–EN6 using the KiCad-derived GPIO map.
- Allocated encoder PIO state machines without colliding with SPI PIO1 SM2.
- Instantiated all six active-low encoder push-buttons.
- Added authoritative signed encoder counts and button bitmaps.

## Protocol implementation

- Preserved the eight-byte envelope, sync byte, protocol version, and CRC-8.
- Added implemented `BUTTON_STATE` message type `0x02`.
- Implemented immutable nine-packet snapshots.
- Added snapshot sequence byte in boundary packet `DATA0`.
- Added typed packet construction and validation functions.

## Systems-programming changes

- Removed project-owned dynamic allocation paths by policy.
- Added scoped enums, fixed-width types, static assertions, checked queues, and
  saturating diagnostics.
- Removed ignored queue results; integrity loss now forces a fresh snapshot.
- Isolated third-party Arduino/Pico usage in hardware-facing modules.

## Verification changes

- Added host tests for CRC, signed 24-bit encoding, fixed queues, commands, and
  full snapshot ordering.
- Added strict host compiler warnings promoted to errors.
- Added a native-source profile checker and GitHub Actions workflow.
- Added profile, matrix, deviation, and determinism documentation.

## Claims deliberately not made

This update does not claim formal MISRA C++:2023 compliance, hard-real-time
qualification, a measured maximum SPI clock, or safety certification.
