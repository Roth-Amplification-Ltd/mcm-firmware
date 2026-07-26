# Determinism and State Machines

## System model

The MCM is a cooperative event-driven embedded system. It is not one enormous
flat switch statement. It is a deterministic superloop coordinating several
explicit finite-state machines and RP2040 PIO hardware state machines.

```text
setup
  initialize transport
  initialize six encoders
  initialize six buttons
  capture initial state
  request initial snapshot

loop
  service SPI FIFOs and frame markers
  process at most one host command
  scan six encoders and six buttons
  queue bounded incremental changes
  advance snapshot/publication FSM by at most one packet
  offer at most one packet to the transport queue
  update application recovery state
```

## Application state machine

```text
Starting
   ├── initialization succeeds → Resynchronizing
   └── initialization fails    → Fault

Running
   ├── host RESYNC             → Resynchronizing
   ├── queue integrity loss    → Resynchronizing
   └── unrecoverable init/API failure → Fault

Resynchronizing
   ├── fresh snapshot fully transmitted → Running
   └── recovery setup fails             → Fault

Fault
   └── remains safe; no uncontrolled state mutation
```

## Button state machine

Each button owns one of six states:

```text
Released
DebouncingPress
Pressed
LongHeld
DebouncingReleasePressed
DebouncingReleaseLong
```

The transitions use unsigned elapsed-time arithmetic, so normal `millis()`
wraparound does not break debounce or long-hold timing.

## Snapshot publication state machine

```text
Idle
Begin
Encoder0
Encoder1
Encoder2
Encoder3
Encoder4
Encoder5
Buttons
End
```

A complete `ControlSnapshot` is copied before `Begin`. Control changes that
occur during serialization are queued after the frozen snapshot and therefore
do not contaminate it.

## SPI receive framing state

The PIO program pushes one received-byte word and then a distinct frame-end
marker after chip select is observed high at a byte boundary. C++ assembly uses
explicit frame state and accepts a command only when exactly eight bytes precede
the marker.

## Bounded execution

A normal loop iteration performs:

- one bounded SPI FIFO service pass;
- no more than one command dispatch;
- exactly six encoder service calls, each capped to a fixed number of samples;
- exactly six button updates;
- no more than six change-enqueue attempts;
- no more than one publisher packet generation;
- no more than one publisher-to-transport enqueue attempt.

This gives structurally bounded execution. It does not replace measurement of
worst-case execution time on the target RP2040.

## Real-time classification

Current defensible description:

> Deterministic cooperative soft-real-time firmware with PIO-assisted wire
> timing and bounded native data structures.

A hard-real-time claim requires measured timing bounds, a maximum supported SPI
clock, stack evidence, and fault-injection results.
