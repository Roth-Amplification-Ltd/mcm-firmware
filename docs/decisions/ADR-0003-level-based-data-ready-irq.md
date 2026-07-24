# ADR-0003: Level-Based Data-Ready IRQ

**Status:** Accepted

## Decision

`SLAVE_IRQ` is active high and represents “response data remains pending.” It is not a momentary event pulse.

## Rationale

A level cannot be missed merely because the master was temporarily busy.

## Consequence

Firmware must deassert IRQ only when no complete response remains to be transferred. Current code approximates this using software queue state and requires hardening to track physical transfer completion.
