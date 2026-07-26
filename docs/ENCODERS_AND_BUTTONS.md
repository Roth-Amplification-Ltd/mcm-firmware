# Encoder and Button Model

## Numbering

Viewed from the component/front side:

```text
EN1 / 0    EN2 / 1    EN3 / 2
EN4 / 3    EN5 / 4    EN6 / 5
```

| Index | Ref | A | B | Button |
|---:|---|---:|---:|---:|
| 0 | EN1 | GPIO4 | GPIO5 | GPIO3 |
| 1 | EN2 | GPIO7 | GPIO6 | GPIO8 |
| 2 | EN3 | GPIO19 | GPIO20 | GPIO18 |
| 3 | EN4 | GPIO1 | GPIO2 | GPIO0 |
| 4 | EN5 | GPIO26 | GPIO27 | GPIO28 |
| 5 | EN6 | GPIO25 | GPIO24 | GPIO23 |

## PIO allocation

| Control | PIO | State machine |
|---|---:|---:|
| EN1 | 0 | 0 |
| EN2 | 0 | 1 |
| EN3 | 0 | 2 |
| EN4 | 0 | 3 |
| EN5 | 1 | 0 |
| EN6 | 1 | 1 |
| Host SPI | 1 | 2 |

PIO1 state machine 3 remains available.

## Quadrature model

The PIO program pushes only changed two-bit A/B states. C++ applies a 16-entry
transition table. Valid adjacent transitions contribute `+1` or `-1`; repeated
or impossible transitions contribute zero. Four valid transitions equal one
reported detent by default.

The internal transition counter saturates at the `int32_t` limits rather than
invoking signed overflow. The wire representation then clamps to signed 24-bit.

## Button model

Each push-button is active low with the RP2040 input pull-up enabled. Its
explicit states are:

```text
Released
DebouncingPress
Pressed
LongHeld
DebouncingReleasePressed
DebouncingReleaseLong
```

Defaults:

```text
debounce: 15 ms
long hold: 600 ms after a debounced press
```

The current pressed and current long-held values are authoritative state, not
one-shot events. This allows a rebooted master to recover from a snapshot.

## Absolute state

The MCM sends absolute encoder counts, not raw direction pulses. If a packet is
lost, the next update or snapshot restores the true state. Reset commands set
software counts to zero; they do not alter the physical encoder mechanism.
