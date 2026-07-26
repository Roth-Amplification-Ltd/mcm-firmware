# Known Limitations

## Qualification gaps

- Formal MISRA C++:2023 compliance is not claimed.
- No qualified static-analysis report or approved deviation dossier exists.
- Worst-case loop execution and stack high-water usage are not measured.
- Maximum supported SPI clock is not measured.
- Hardware-in-loop conformance and fault-injection tests are not yet automated.
- Watchdog, brownout, and persistent reset-cause reporting are not implemented.

## Transport limitations

The combined PIO SPI RX/TX design requires CPU FIFO servicing during a frame.
At an excessive master clock, TX may repeat stale OSR data or RX may stall.
The canonical C++ layer detects a frame that completed before all eight TX bytes
were staged, records `txUnderrun`, clears stale publication state, and forces a
new snapshot. This recovery does not make an excessive SPI clock valid; the
clock still must be bounded experimentally or the transport redesigned with
DMA/two state machines.

The PIO frame marker is recognized at a byte boundary. The protocol forbids
mid-byte CS aborts; malformed electrical transactions require target testing.

## Protocol limitations

- `SET_LED` and `GET_INFO` are reserved but unsupported.
- Diagnostics are not remotely readable.
- Snapshot sequence is one byte and may wrap.
- There is no authentication; the bus is assumed to be internal to the pedal.

## Application limitations

- Encoder values are generic signed counts, not user-configurable parameter
  ranges, acceleration profiles, or semantic labels.
- Reset commands zero software counts and may not match a host's desired
  parameter default without a higher-level mapping layer.
- The current application uses only core 0 and no interrupts for native logic.
