# ADR-0007: Canonical Deterministic MISRA-Like Native Firmware

- Status: Accepted
- Date: 2026-07-26

## Context

The repository contained a corrected SPI transport and a separate historical
integration tree, preventing one obvious build target and making deterministic
analysis difficult. The project also wanted higher-assurance embedded practices
without making an unsupported formal MISRA compliance claim.

## Decision

Create one canonical sketch at `src/MCM_Firmware/` and adopt the documented
MISRA C++:2023-inspired native-code profile. Preserve older trees under
`archive/`. Use explicit finite-state machines, fixed storage, checked bounded
queues, immutable snapshots, typed packet construction, diagnostics, and CI
checks. Treat Arduino-Pico, Pico SDK, PIO assembly, and generated PIO headers as
adopted/generated code.

## Consequences

- The firmware has one obvious production source target.
- Host-testable logic is separated from hardware-facing code.
- Manufacturers receive a more auditable reference implementation.
- Formal MISRA compliance remains possible later but still requires licensed
  guidance, a configured analyzer, deviations, and qualification evidence.
- Archived code remains available but is no longer an active build target.
