# Production Qualification Roadmap

## Completed by the deterministic hardening pass

- one canonical sketch;
- six encoders and six buttons integrated;
- explicit finite-state machines;
- immutable snapshots;
- checked bounded queues;
- deterministic RESYNC;
- host protocol tests;
- automated native profile check;
- adopted-code/deviation documentation.

## Next: target build and board bring-up

1. Compile the canonical sketch with the pinned Arduino-Pico version.
2. Resolve any target-only warnings or PIO syntax/generation issues.
3. Flash one MCM board and validate all six encoder directions and buttons.
4. Capture startup, snapshot, incremental update, reset, and RESYNC transactions.

## Next: timing qualification

1. Measure worst/average main-loop execution.
2. Sweep SPI clock until errors occur under simultaneous encoder activity.
3. Set a conservative normative maximum SPI clock.
4. Measure stack usage and prolonged-operation stability.
5. Test FIFO pressure and IRQ latency.

## Next: robustness

1. Add watchdog and reset-cause diagnostics.
2. Add remote diagnostic/info packets.
3. Add automated malformed-frame and brownout tests.
4. Decide whether DMA or split PIO state machines are needed.
5. Add release-version and hardware-revision discovery.

## Next: formal compliance option

1. Acquire/select the MISRA C++:2023 guideline and analyzer.
2. Freeze native/adopted/generated scopes.
3. Classify applicable rules.
4. analyze the exact target build configuration;
5. resolve findings and approve deviations;
6. produce a guideline compliance summary;
7. retain tool, compiler, test, and review evidence per release.
