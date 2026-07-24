# Known Limitations and Correctness Risks

## Repository structure

1. The newer root sketch includes support files that are absent from the same source directory.
2. The nested Layer-A directory contains two Arduino `.ino` entry points.
3. Root and nested copies of SPI transport and PIO code have diverged.
4. The corrected root PIO file and hardware map are not yet combined with the more complete nested protocol stack.

## Control scanning

5. EN1–EN6 are not instantiated by the active sketch.
6. The button debounce objects are not instantiated by the active sketch.
7. Placeholder parameters do not correspond to the six hardware controls.
8. No application binding converts encoder detents into parameter values.
9. Button state and button edges are not published on the current wire protocol.

## Snapshot behavior

10. Current snapshot generation does not capture an immutable control-state buffer.
11. The proposed complete snapshot contains nine packets, but the software TX queue depth is eight.
12. Snapshot flow is released when END is serialized, not when END is physically transferred.
13. RESYNC clears the software TX queue but not bytes already loaded in the PIO TX FIFO.

## IRQ and TX ownership

14. Packets are removed from the software queue when handed to the PIO FIFO.
15. IRQ can deassert before the master physically clocks the last queued packet.
16. `pull noblock` repeats prior OSR contents when TX FIFO is empty; idle response bytes are not formally defined.
17. Queue overflow is returned as `false` but the current caller discards the result.

## RX framing

18. Only one completed RX packet slot exists.
19. A new CS assertion can clear a previous unread RX-ready state.
20. CS edges are observed by main-loop polling, which can miss a short complete transaction if service latency is excessive.
21. Extra bytes within one CS frame are discarded but no diagnostic counter is maintained.

## Event queue

22. Event queue overflow is silent unless the caller checks `push()`.
23. Queue operations are not interrupt-safe or multicore-safe.
24. Reset-all can partially enqueue when the queue is too small, leaving the master with an incomplete report.

## PIO encoder implementation

25. Each `EncoderPIO::begin()` loads another copy of the PIO program instead of sharing one program offset per PIO block.
26. Six encoder state machines must be allocated across both PIO blocks.
27. No resource allocator prevents state-machine or instruction-memory collisions.
28. The encoder PIO loop has no explicit input shift-register clearing commentary in the original code and requires hardware validation.

## Testing and release engineering

29. No CI build is configured.
30. No committed known-good logic-analyzer traces exist.
31. No automated protocol compatibility/version test gates changes.
32. No released firmware image is tied to a documented protocol revision.

These limitations do not invalidate the architecture. They define the work required before calling the firmware production-ready.
