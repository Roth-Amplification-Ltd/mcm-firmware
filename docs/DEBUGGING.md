# Debugging and Logic Analyzer Guide

## Minimum capture

Capture:

```text
GPIO11 IRQ
GPIO12 SCK
GPIO13 CS
GPIO14 MISO
GPIO15 MOSI
```

Decode as SPI Mode 0, MSB first, eight bytes per CS frame.

## Expected snapshot

After GET_SNAPSHOT or RESYNC, expect nine validated response frames in this
order:

```text
10, 01, 01, 01, 01, 01, 01, 02, 11
```

The six `0x01` packets must have indexes 0 through 5. BEGIN and END must carry
the same sequence byte in DATA0.

## Fault isolation

| Symptom | Check |
|---|---|
| IRQ never rises | publisher request, TX enqueue, GPIO11 direction |
| IRQ never falls | frame-end marker, CS rising at byte boundary, TX retirement |
| repeated MISO byte | TX FIFO starvation at excessive SPI clock |
| missing command | frame length, marker delivery, RX overwrite counter |
| reversed encoder direction | A/B map and physical footprint orientation |
| encoder never moves | adjacent pin assertion, PIO allocation, generated header |
| button chatters | raw wiring, pull-up, debounce timing |
| snapshot sequence breaks | queue recovery, master transaction count, CRC rejection |

## Diagnostics currently available in memory

- bad sync/version/CRC;
- short/long SPI frame;
- RX overwrite;
- publication queue overflow;
- TX queue overflow;
- TX underrun/unstaged packet at frame completion;
- unsupported command;
- invalid control index;
- encoder initialization failure;
- RESYNC count.

A later protocol extension should expose these counters through `GET_INFO` or a
separate diagnostics response.
