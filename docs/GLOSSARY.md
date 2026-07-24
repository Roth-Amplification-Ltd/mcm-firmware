# Glossary

**Authoritative state** — The MCM's accepted parameter/control value. Outbound packets report this state; they do not create a second state store.

**CS frame** — One active-low chip-select interval containing exactly eight transferred bytes.

**Detent** — A mechanical click position of a rotary encoder. One detent may produce multiple quadrature transitions.

**Event** — Internal record indicating that a state packet or snapshot marker should be produced.

**IRQ** — `SLAVE_IRQ`, the active-high MCM-to-master signal indicating pending response data.

**MCM** — Modular Control Module.

**PIO** — RP2040 Programmable I/O peripheral.

**Protocol index** — Zero-based control/parameter number carried in packet byte 3.

**RESYNC** — Master command requesting stale output be discarded and a fresh framed snapshot be generated.

**Snapshot** — Ordered packet sequence representing a baseline state. The design-target coherent snapshot captures all controls before transmission.

**State publisher** — Module that serializes internal events into wire packets.

**Transport** — SPI/PIO layer responsible for electrical byte movement and framing, not command semantics.
