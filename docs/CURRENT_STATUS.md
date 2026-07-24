# Current Repository Status

**Audited branch:** `main`  
**Audited commit:** `0734fb17a53a8457c1db6c18560c552f6e7ac451` (`Align firmware with SPI-only MCM hardware`)

## Executive summary

The repository contains a corrected hardware-aligned SPI transport at the top of `src/` and a more complete historical command/state implementation inside `src/Modular_Control_Module_PIO_SPI_LAYER_A_RX/`. The two trees are not presently consolidated into one clean Arduino sketch.

The architectural ideas are useful and largely consistent:

- SPI Mode 0, MSB-first;
- fixed eight-byte packets;
- CRC-8 validation;
- CS-framed commands;
- level-based data-ready IRQ;
- event-to-packet state publishing;
- framed snapshots and a RESYNC command.

The physical encoder and button scanning is not connected to the live parameter table, and the source layout currently prevents a straightforward clean build.

## Build-status matrix

| Candidate | Useful content | Blocking problem | Classification |
|---|---|---|---|
| `src/Modular_Control_Module_PIO_SPI.ino` | Corrected SPI transport wiring and centralized hardware map | Includes `EventQueue.h`, `TransportProtocol.h`, `StatePublisher.h`, `ParamBinding.h`, and `CommandDispatcher.h`, but those files are no longer present beside the sketch | Incomplete integration sketch |
| `src/Modular_Control_Module_PIO_SPI_LAYER_A_RX/Modular_Control_Module_PIO_SPI_LAYER_A_RX.ino` | Command dispatcher, snapshot flow, RESYNC path, TX queue | Same directory also contains `Modular_Control_Module.ino`, giving Arduino two `setup()` and two `loop()` definitions; nested PIO code is stale relative to the corrected root PIO code | Historical integration reference |
| Root `TransportSPI.*` + `SpiSlaveM0.pio` | Best current SPI hardware mapping; corrected absolute GPIO waits and configured `jmp pin` | No complete application layer beside it | Canonical transport slice |
| Nested protocol/dispatcher/publisher files | Most complete current protocol logic | Hard-coded pins, stale transport copy, placeholder parameters, no live six-control scanning | Canonical logic reference pending consolidation |

## Implemented today

- External SPI-only hardware mapping: GPIO11 through GPIO15.
- SPI Mode 0, MSB-first PIO transport.
- Eight-byte software packet envelope.
- Level-based IRQ driven from TX queue state.
- CRC-8 helper using polynomial `0x07`.
- Message IDs for parameter state, snapshot begin/end, reset commands, and RESYNC.
- Internal event ring buffer.
- Parameter reset behavior.
- Snapshot overlap guard.
- Standalone encoder PIO decoder class.
- Standalone debounced-button class.

## Not connected or not complete

- Six physical encoders are not instantiated in the active sketch.
- Six push-buttons are not instantiated in the active sketch.
- The parameter table contains placeholders rather than EN1–EN6.
- Encoder/button changes do not currently generate events in the active sketch.
- No `BUTTON_STATE` packet is implemented in the code on `main`.
- Snapshot generation reads the current parameter array while enqueueing; it does not capture an immutable six-control sample first.
- The software TX queue depth is eight packets, while the proposed full encoder/button snapshot contains nine packets.
- IRQ is deasserted when the software queue is emptied into PIO, not necessarily when the final bit is physically clocked by the master.
- `clearTxQueue()` does not clear already-loaded PIO FIFO bytes.
- No host-buildable unit-test system is wired into CI.
- No single canonical Arduino sketch directory exists.

## Documentation policy

Documents use three labels:

- **Implemented:** behavior visible in code on `main`.
- **Design target:** behavior already agreed or specified but not completely implemented.
- **Recommendation:** proposed engineering work that has not yet been accepted as protocol behavior.

This distinction prevents a master-firmware developer from implementing against features that the current MCM binary cannot yet provide.
