# MCM MISRA C++:2023-Inspired Native-Code Profile

## Status and claim boundary

This document defines the enforceable **MCM native-code profile**. It borrows
objectives and practices associated with MISRA C++:2023, CERT C++, defensive
embedded programming, and deterministic state-machine design.

The project does **not** claim formal MISRA C++:2023 compliance. The word
“MISRA-like” means the native project code has been shaped to make later formal
analysis practical and to provide useful discipline today.

## Scope

Included native code:

```text
src/MCM_Firmware/*.h
src/MCM_Firmware/*.cpp
src/MCM_Firmware/*.ino
```

Separately reviewed adopted/generated code:

- Arduino-Pico core;
- Raspberry Pi Pico SDK headers and libraries;
- RP2040 hardware register definitions;
- generated `.pio.h` files;
- PIO assembly sources;
- boot ROM and toolchain runtime.

Archived source is outside the active scope.

## Mandatory project rules

### Language and representation

1. Native code uses the C++17 language subset supported by Arduino-Pico.
2. Interface-visible integers use `<cstdint>` fixed-width types.
3. Enumerations are scoped with `enum class` and fixed underlying types.
4. Wire data is assembled byte-by-byte; compiler structure layout is never sent.
5. Signed 24-bit values are range-clamped before serialization.
6. Array sizes and protocol constants are compile-time constants.
7. Critical representation assumptions use `static_assert`.

### Memory and lifetime

1. No project-owned `new`, `delete`, `malloc`, `calloc`, `realloc`, or `free`.
2. No Arduino `String` or dynamically sized STL containers.
3. No recursion.
4. Application objects and queues have static storage or bounded automatic
   storage.
5. Pointers are restricted to adopted hardware APIs and are not used for
   ownership.

### Control flow and state

1. Application behavior is driven by one cooperative superloop.
2. Every persistent protocol or user-input process has an explicit state type.
3. Each loop iteration performs bounded work.
4. Queue insertion failures are never ignored.
5. Invalid packets cannot mutate application state.
6. Recovery clears stale publication state and emits a fresh atomic snapshot.
7. A snapshot is copied before serialization and cannot mix sample times.

### Concurrency

1. Native application objects are owned by core 0 and the main execution
   context.
2. No native queue may be accessed concurrently from an ISR or core 1.
3. `volatile` is not used as a synchronization mechanism.
4. Any future interrupt/core sharing requires a new documented synchronization
   design and test plan.

### Error handling

1. Exceptions are not used.
2. Fallible operations return typed status or `bool` marked `[[nodiscard]]`.
3. Recoverable faults increment saturating diagnostic counters.
4. Loss of publication integrity forces a deterministic fresh-snapshot recovery.
5. Unsupported commands are counted and otherwise have no side effects.

### Build and review

1. All native source files carry MPL-2.0 SPDX headers.
2. Host-testable modules compile with strict warnings promoted to errors.
3. CI runs the license checker, profile checker, and host tests.
4. Protocol changes require byte-level tests.
5. Transport changes require target-hardware and logic-analyzer evidence.
6. Deviations from this profile are recorded in `MISRA_DEVIATIONS.md`.

## Prohibited native-code constructs

The automated checker rejects project-owned uses of:

- dynamic allocation APIs;
- Arduino `String`;
- dynamic STL containers;
- exceptions;
- RTTI downcasts;
- `goto`;
- unscoped enumerations;
- ignored-return `(void)` casts;
- tabs and trailing whitespace.

The checker is deliberately limited. It is not a replacement for a commercial
MISRA analyzer or expert review.
