# Master MCU Integration Guide

## Wiring

Connect common ground and 3.3 V-compatible signals. Do not connect a 5 V SPI master directly.

| Master signal | MCM signal |
|---|---|
| SCK | GPIO12 / `SPI_SCK` |
| chip select output | GPIO13 / `SPI_CS` |
| master input | GPIO14 / `SPI_MISO` |
| master output | GPIO15 / `SPI_MOSI` |
| interrupt-capable input | GPIO11 / `SLAVE_IRQ` |

## SPI configuration

```text
Mode: 0
Bit order: MSB first
Frame: 8 bytes
CS: low for exactly one frame
```

Start at a conservative clock rate and increase only after logic-analyzer verification. The repository does not yet declare a tested maximum SCK frequency.

## Command sequence

1. Build an eight-byte command.
2. Compute CRC over bytes 0–6.
3. Assert CS.
4. Transfer exactly eight bytes.
5. Deassert CS.
6. Wait for IRQ if a response is expected.
7. Clock one dummy eight-byte frame per response packet.
8. Validate sync, version, type, index, and CRC before accepting data.

## Dummy read frames

Use a MOSI pattern that cannot accidentally validate as a command, such as eight zero bytes. A valid command requires sync byte `0xA5`, so all-zero dummy input is rejected by the dispatcher.

## Snapshot parser

The parser should maintain a temporary candidate snapshot. Publish it to the host application only after a valid matching END packet and completeness checks.

Do not update individual application controls immediately as packets arrive; doing so exposes partial state when a later packet fails CRC.

## RESYNC recovery

Recommended recovery after timeout, CRC error, unexpected message, or host reboot:

1. Send `MSG_CMD_RESYNC`.
2. Discard all returned data until a valid `SNAPSHOT_BEGIN` is seen.
3. Collect the new framed snapshot.
4. Atomically replace the host's state only after valid END.

This discard step matters because current firmware may leave stale bytes in the PIO TX FIFO even after clearing the software queue.

## Parser limits

Always impose:

- IRQ wait timeout;
- maximum number of packets per snapshot;
- duplicate-index rejection;
- missing-index rejection;
- version check;
- CRC check;
- valid index bounds;
- reserved-bit validation when practical.

## Interrupt handling

Keep the IRQ ISR minimal. Record that data is pending and perform SPI transactions in the normal task or main loop unless the host platform explicitly supports safe SPI use inside the interrupt context.

## Example implementation

A complete Arduino-style master example and portable protocol header are included in [`protocol/snapshot-v1.1/`](protocol/snapshot-v1.1/). Remember that the full button snapshot behavior is a design target until MCM firmware implements it.
