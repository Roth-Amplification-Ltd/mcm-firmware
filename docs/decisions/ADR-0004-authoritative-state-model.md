# ADR-0004: MCM Owns Authoritative State

**Status:** Accepted

## Decision

The MCM converts raw electrical inputs into stable parameter and button state. The master consumes absolute values and snapshots rather than decoding quadrature edges or switch bounce.

## Consequences

- Local scanning behavior is hidden behind a stable protocol.
- Reset commands mutate MCM state and then report it.
- Packet loss is recovered by requesting a baseline snapshot.
