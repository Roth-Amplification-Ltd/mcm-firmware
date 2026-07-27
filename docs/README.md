# MCM Firmware Documentation

This directory contains the maintained engineering documentation for the
Modular Control Module firmware.

## Core documentation

| Document | Purpose |
|---|---|
| [Current Status](CURRENT_STATUS.md) | Implemented behavior, qualification status, and remaining risks |
| [Repository Layout](REPOSITORY_LAYOUT.md) | Active source tree and supporting project directories |
| [Architecture](ARCHITECTURE.md) | Module boundaries, state ownership, queues, and data flow |
| [Determinism and State Machines](DETERMINISM.md) | Cooperative scheduling, bounded work, and timing claims |
| [Firmware Engineering Profile](MISRA_LIKE_PROFILE.md) | Enforced native-code rules and formal-compliance boundary |
| [Compliance Matrix](MISRA_COMPLIANCE_MATRIX.md) | Evidence status by engineering objective |
| [Deviation and Adopted-Code Register](MISRA_DEVIATIONS.md) | Arduino-Pico, Pico SDK, generated PIO, and low-level exceptions |
| [Hardware and Control Map](HARDWARE.md) | SPI connector and every encoder/button GPIO |
| [SPI Wire Protocol](SPI_PROTOCOL.md) | Normative packet format and message definitions |
| [Encoder and Button Model](ENCODERS_AND_BUTTONS.md) | Numbering, quadrature, debounce, and snapshot behavior |
| [PIO Implementation](PIO_IMPLEMENTATION.md) | SPI and encoder PIO behavior |
| [Build and Flash](BUILD_AND_FLASH.md) | Toolchain setup, build target, and upload workflow |
| [Master Integration](MASTER_INTEGRATION.md) | Host transaction sequencing and parser requirements |
| [Debugging](DEBUGGING.md) | Diagnostics, logic-analyzer captures, and fault isolation |
| [Known Limitations](KNOWN_LIMITATIONS.md) | Remaining technical and qualification risks |
| [Implementation Roadmap](IMPLEMENTATION_ROADMAP.md) | Work required for production qualification |
| [Maintainer Checklist](MAINTAINER_CHECKLIST.md) | Required updates when changing pins, protocol, or state behavior |
| [Glossary](GLOSSARY.md) | Project terminology |
| [Licensing](LICENSING.md) | MPL-2.0 scope, distribution obligations, and contributions |

## Protocol reference

[`protocol/snapshot-v1.1/`](protocol/snapshot-v1.1/) contains portable protocol
headers, test vectors, a decoder, and host integration examples.

## Architectural decisions

Major interface and architecture decisions are recorded in
[`decisions/`](decisions/).
