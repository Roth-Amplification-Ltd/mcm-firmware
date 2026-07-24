# Debugging and Logic Analyzer Guide

## Minimum capture channels

Capture:

1. `SPI_CS`
2. `SPI_SCK`
3. `SPI_MOSI`
4. `SPI_MISO`
5. `SLAVE_IRQ`

Use a sample rate comfortably above the SPI clock. Decode as Mode 0, MSB-first, eight bits per word.

## Expected transaction shape

```text
CS:   high ____| low for 64 clocks |____ high
SCK:       ____/\/\/\/...64 edges...\/_
MOSI:      eight command/dummy bytes
MISO:      eight response/previous bytes
IRQ:  high while response packet(s) remain pending
```

## First checks

- CS returns high between every eight-byte packet.
- SCK is low when CS asserts.
- exactly 64 clocks occur per frame.
- first MISO bit is stable before the first rising edge.
- byte 0 of a response is `0xA5`.
- CRC recomputes correctly.
- IRQ does not pulse too briefly for the host.

## Common failures

### All `0xFF` or all `0x00` on MISO

Check MISO pin direction, PIO state-machine enable, PIO program/header match, common ground, and whether TX FIFO data was loaded before the first clock.

### One-bit shift

Check first-bit preload and Mode-0 edge assumptions. The master samples on rising edge; MISO must change on falling edge and be preloaded before the first rising edge.

### Correct bytes but rejected command

Recompute CRC over exactly bytes 0–6. Verify version `0x01`, sync `0xA5`, and that byte order was not reversed by a word-oriented SPI API.

### Partial packet accepted or full packet discarded

Check CS framing and main-loop service frequency. Current C++ code recognizes CS edges by polling `digitalRead()`.

### IRQ low while final packet still clocks

This matches a known current limitation: software queue retirement occurs when bytes are loaded into PIO FIFO, not when physically shifted.

### Encoder direction reversed

Swap caller A/B mapping for that encoder rather than changing the global transition table. EN2 and EN6 GPIO order is reversed numerically, which `canonicalize()` is designed to handle.

### Button flicker

Inspect raw input, pull-up, ground reference, debounce interval, and update frequency. Confirm `update()` is called continuously.

## Serial logging policy

Useful counters:

- valid RX packets;
- bad sync;
- bad version;
- bad CRC;
- partial CS frames;
- RX overlength frames;
- event-queue overflow;
- TX-queue overflow;
- RESYNC count;
- snapshot count;
- encoder invalid transitions;
- control-scan deadline misses.

Avoid printing from time-critical PIO service paths at high rates. Aggregate counters and print periodically.
