# MCM Firmware Documentation Index

This directory is the maintained engineering handbook for the Modular Control
Module firmware.

## Core documents

| Document | Purpose |
|---|---|
| [Current Repository Status](CURRENT_STATUS.md) | Canonical build target, completed hardening work, and remaining risks |
| [Repository Layout](REPOSITORY_LAYOUT.md) | File-by-file map of the canonical and archived source trees |
| [Architecture](ARCHITECTURE.md) | Module boundaries, state ownership, queues, and end-to-end data flow |
| [Determinism and State Machines](DETERMINISM.md) | Cooperative scheduling, finite-state machines, bounded work, and timing claims |
| [MISRA-like Native-Code Profile](MISRA_LIKE_PROFILE.md) | Enforced project coding profile and formal-compliance boundary |
| [MISRA-like Compliance Matrix](MISRA_COMPLIANCE_MATRIX.md) | Evidence status by engineering objective |
| [Deviation and Adopted-Code Register](MISRA_DEVIATIONS.md) | Arduino-Pico, Pico SDK, generated PIO, and low-level deviations |
| [Hardware and Control Map](HARDWARE.md) | SPI connector and every encoder/button GPIO |
| [SPI Wire Protocol](SPI_PROTOCOL.md) | Normative eight-byte packet and message IDs |
| [Encoder and Button Model](ENCODERS_AND_BUTTONS.md) | Numbering, quadrature, debounce, and snapshot behavior |
| [PIO Implementation](PIO_IMPLEMENTATION.md) | SPI and encoder PIO instruction walkthroughs |
| [Build and Flash](BUILD_AND_FLASH.md) | Toolchain setup, build target, and upload workflow |
| [Master Integration](MASTER_INTEGRATION.md) | Host MCU transaction sequencing and parser requirements |
| [Debugging](DEBUGGING.md) | Serial diagnostics, logic-analyzer captures, and fault isolation |
| [Known Limitations](KNOWN_LIMITATIONS.md) | Technical debt and correctness risks that remain |
| [Implementation Roadmap](IMPLEMENTATION_ROADMAP.md) | Recommended order for production qualification |
| [Maintainer Checklist](MAINTAINER_CHECKLIST.md) | Required updates when changing pins, messages, or state behavior |
| [Glossary](GLOSSARY.md) | Project terminology |
| [Licensing](LICENSING.md) | MPL-2.0 scope, distribution obligations, contributions, and hardware separation |

## Protocol reference package

The directory [`protocol/snapshot-v1.1/`](protocol/snapshot-v1.1/) contains the
portable protocol reference materials. The canonical firmware now implements
the same six-encoder/six-button nine-packet snapshot shape, while the reference
package remains useful for host integration and test vectors.

## Decision records

Architectural decisions are recorded in [`decisions/`](decisions/). Add a new
ADR rather than burying major interface decisions in a commit message.
