# Current Repository Status

## Executive summary

The repository now contains one canonical Arduino-Pico sketch at
`src/MCM_Firmware/MCM_Firmware.ino`. The previous split root transport and
historical Layer-A/Layer-D sketch trees are preserved under
`archive/pre-misra-canonicalization/` and excluded from the native-code quality
scope.

The canonical implementation provides:

- six live PIO-backed rotary encoders;
- six explicit finite-state-machine button debouncers;
- immutable control snapshots;
- fixed eight-byte SPI packets with CRC-8;
- a nine-packet encoder/button snapshot;
- checked bounded queues;
- explicit application, snapshot, and receive-frame states;
- deterministic RESYNC recovery;
- diagnostic counters, including detected TX underrun;
- no project-owned dynamic allocation;
- host-buildable protocol and snapshot tests;
- automated MPL and MISRA-like profile checks.

## Classification

| Area | Classification |
|---|---|
| Canonical sketch layout | Implemented |
| Six encoder scanning | Implemented |
| Six button scanning | Implemented |
| Atomic snapshot capture | Implemented |
| Button-state packet | Implemented |
| Explicit application FSM | Implemented |
| Queue failure recovery | Implemented |
| Native-code MISRA-like profile | Implemented |
| Formal MISRA C++:2023 compliance claim | Not claimed |
| Worst-case execution-time proof | Not complete |
| Maximum supported SPI clock proof | Not complete |
| Qualified static-analysis report | Not complete |
| Hardware-in-loop conformance report | Not complete |

## Important remaining limitations

1. The SPI PIO transport still depends on the CPU servicing split RX/TX FIFOs
   quickly enough for the selected SPI clock. A measured maximum clock and
   logic-analyzer evidence are still required.
2. PIO assembly and generated PIO headers are outside the native C++ rule scope
   and require separate review and test evidence.
3. Arduino-Pico and the Pico SDK are adopted code. Their behavior is isolated
   behind project modules but is not claimed as MISRA-conforming source.
4. Diagnostic counters are internal and are not yet exposed through a
   `GET_INFO` or diagnostics protocol response.
5. Stack usage, loop worst-case timing, and brownout/watchdog recovery remain to
   be measured on target hardware.

## Terminology

- **Implemented:** behavior visible in the canonical source tree.
- **MISRA-like:** project rules inspired by MISRA objectives but not a formal
  compliance statement.
- **Adopted code:** third-party or generated code used by the product but not
  maintained under the complete native-code rule set.
- **Qualification target:** evidence still required before production claims.
