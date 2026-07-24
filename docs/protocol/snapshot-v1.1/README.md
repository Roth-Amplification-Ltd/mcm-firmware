# Coherent Six-Control Snapshot Protocol v1.1 — Design Target

This directory contains a complete proposed protocol for sampling all six encoder values and all six push-buttons as one coherent state update.

It includes:

- normative specification;
- packet diagram;
- KiCad-verified control map;
- portable C/C++ protocol header;
- Arduino-style master example;
- CRC and decode self-test;
- canonical test vectors;
- Python decoder.

## Implementation warning

The firmware currently checked into `main` does **not** implement the full contents of this proposal. Specifically, it does not emit `MSG_BUTTON_STATE`, does not capture one immutable six-control snapshot before serialization, does not transmit the proposed sequence ID/status fields, and does not guarantee IRQ remains high until the final packet is physically shifted.

Use this package as the implementation target and interoperability test suite, not as proof of current binary behavior.
