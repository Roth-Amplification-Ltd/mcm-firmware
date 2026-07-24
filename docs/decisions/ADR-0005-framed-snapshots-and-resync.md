# ADR-0005: Framed Snapshots and RESYNC

**Status:** Accepted concept; full coherent control snapshot is not yet implemented

## Decision

A baseline is an ordered `SNAPSHOT_BEGIN`, state records, and `SNAPSHOT_END` sequence. `RESYNC` discards stale pending software output and requests a fresh snapshot.

## Consequences

- Snapshots must not overlap or interleave.
- Masters validate completeness before publishing state.
- Firmware must eventually handle stale PIO FIFO bytes and maintain IRQ through physical END transfer.
