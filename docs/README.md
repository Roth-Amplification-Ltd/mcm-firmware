# MCM Firmware Documentation Index

This directory is the maintained engineering handbook for the Modular Control Module firmware.

## Core documents

| Document | Purpose |
|---|---|
| [Current Repository Status](CURRENT_STATUS.md) | What builds, what does not, and why the source tree is currently split |
| [Repository Layout](REPOSITORY_LAYOUT.md) | File-by-file map of both source trees |
| [Architecture](ARCHITECTURE.md) | Module boundaries, state ownership, queues, and end-to-end data flow |
| [Hardware and Control Map](HARDWARE.md) | SPI connector and every encoder/button GPIO |
| [SPI Wire Protocol](SPI_PROTOCOL.md) | Normative eight-byte packet and current message IDs |
| [Encoder and Button Model](ENCODERS_AND_BUTTONS.md) | Numbering, quadrature, debounce, and snapshot behavior |
| [PIO Implementation](PIO_IMPLEMENTATION.md) | SPI and encoder PIO instruction walkthroughs |
| [Build and Flash](BUILD_AND_FLASH.md) | Toolchain setup, sketch layout requirements, build and upload workflow |
| [Master Integration](MASTER_INTEGRATION.md) | Host MCU transaction sequencing and parser requirements |
| [Debugging](DEBUGGING.md) | Serial diagnostics, logic-analyzer captures, and fault isolation |
| [Known Limitations](KNOWN_LIMITATIONS.md) | Technical debt and correctness risks in current code |
| [Implementation Roadmap](IMPLEMENTATION_ROADMAP.md) | Recommended order for turning the repository into production firmware |
| [Maintainer Checklist](MAINTAINER_CHECKLIST.md) | Required updates when changing pins, messages, or state behavior |
| [Glossary](GLOSSARY.md) | Project terminology |

## Protocol proposal package

The directory [`protocol/snapshot-v1.1/`](protocol/snapshot-v1.1/) contains the previously designed coherent six-encoder/six-button snapshot extension, reference header, master example, test vectors, and decoder.

**Important:** that proposal is more complete than the code currently present on `main`. In particular, `MSG_BUTTON_STATE`, immutable control capture, sequence IDs, and final-packet IRQ semantics are not fully implemented by the present firmware.

## Decision records

Architectural decisions are recorded in [`decisions/`](decisions/). Add a new ADR rather than burying major interface decisions in a commit message.
