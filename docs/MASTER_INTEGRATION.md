# Master Integration Guide

## Startup

1. Configure SPI Mode 0, MSB first, 3.3 V.
2. Configure `SLAVE_IRQ` as an active-high input.
3. Send a valid `RESYNC` or `GET_SNAPSHOT` command.
4. While IRQ is high, clock one eight-byte frame at a time using a valid NOP on
   MOSI.
5. Validate every MISO packet before applying it.
6. Commit snapshot state only after matching BEGIN and END sequence bytes.

## Parser rules

The master must reject packets with:

- wrong sync;
- unsupported version;
- incorrect CRC;
- unknown required semantics;
- invalid control index;
- malformed snapshot ordering;
- mismatched sequence bytes.

Incremental encoder/button packets received outside a snapshot may be applied
immediately after validation.

## Snapshot buffering

Do not apply snapshot packets directly to live UI state. Buffer six encoder
values and the button packet, then atomically replace the master-side baseline
when a valid END arrives.

## IRQ

IRQ is level based. Do not treat it as a one-shot pulse. Continue reading until
it deasserts. The canonical transport keeps IRQ asserted until a staged packet
has completed a CS frame and all queued packets are consumed.

## Recovery

Send `RESYNC` after:

- master reboot;
- CRC burst;
- malformed snapshot;
- sequence mismatch;
- timeout while IRQ remains active;
- hot-plug or brownout suspicion.

A RESYNC intentionally discards stale outbound state and establishes a new
baseline.
