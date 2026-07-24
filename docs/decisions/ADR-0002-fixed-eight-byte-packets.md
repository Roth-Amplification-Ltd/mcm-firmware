# ADR-0002: Fixed Eight-Byte Packets

**Status:** Accepted

## Decision

Every CS assertion transfers exactly eight bytes containing sync, version, type, index, three payload bytes, and CRC.

## Rationale

Fixed framing simplifies PIO transport, master parsing, resynchronization, and bounded memory use.

## Consequences

- Larger responses use ordered packet sequences.
- The master must clock exactly 64 bits per frame.
- Protocol changes preserve the envelope or increment the version.
