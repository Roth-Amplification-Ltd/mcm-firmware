# Current Firmware Status

## Implemented

The active firmware provides:

- six PIO-backed rotary encoders;
- six debounced push-buttons with long-hold state;
- authoritative absolute control state;
- immutable state snapshots;
- fixed eight-byte SPI packets with CRC-8;
- nine-packet encoder/button snapshots;
- explicit application, snapshot, button, and receive-frame state machines;
- fixed-capacity queues with checked failures;
- deterministic `RESYNC` recovery;
- transport and recovery diagnostic counters;
- no project-owned dynamic allocation;
- host-buildable protocol and state-machine tests;
- automated licensing and native-source profile checks.

## Qualification status

| Area | Status |
|---|---|
| Active sketch layout | Implemented |
| Six encoder scanning | Implemented |
| Six button scanning | Implemented |
| Atomic snapshot capture | Implemented |
| Button-state packet | Implemented |
| Explicit application state machine | Implemented |
| Queue-failure recovery | Implemented |
| Native-source deterministic profile | Implemented |
| Formal MISRA C++:2023 compliance | Not claimed |
| Worst-case execution-time proof | Not complete |
| Maximum supported SPI clock | Not complete |
| Qualified static-analysis report | Not complete |
| Hardware-in-loop conformance report | Not complete |

## Remaining engineering work

1. Measure and publish the maximum supported SPI clock.
2. Capture logic-analyzer evidence for framing, IRQ behavior, and underrun recovery.
3. Measure worst-case loop time and stack high-water use on target hardware.
4. Validate encoder direction, detent scaling, and button timing on production hardware.
5. Validate watchdog, brownout, reset, and host-disconnect recovery.
6. Expose diagnostic counters through a defined protocol response.
7. Run a qualified MISRA-capable analyzer and approve any required deviations
   before making a formal compliance claim.

## Terms

- **Implemented:** behavior present in the active firmware.
- **Engineering profile:** project rules enforced by source checks and review.
- **Adopted code:** Arduino-Pico, Pico SDK, or generated code outside the complete
  project-owned native-source rule scope.
- **Qualification:** target-hardware evidence required before production claims.
