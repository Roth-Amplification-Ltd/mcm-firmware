# Encoder and Button Model

## Numbering

Protocol index and button bit positions use the same zero-based order:

| Index/bit | Control |
|---:|---|
| 0 | EN1, top left |
| 1 | EN2, top center |
| 2 | EN3, top right |
| 3 | EN4, bottom left |
| 4 | EN5, bottom center |
| 5 | EN6, bottom right |

## Quadrature decoding

Each mechanical encoder exposes phase A and phase B. Legal quadrature motion changes one bit at a time through a Gray-code sequence. The firmware's lookup table maps `(previous_state << 2) | new_state` to `-1`, `0`, or `+1`.

Invalid two-bit jumps produce zero delta. This rejects many bounce and missed-edge artifacts but cannot reconstruct motion that was never sampled.

## PIO state stream

`EncoderPIO.pio` continuously samples two adjacent GPIOs and pushes a value only when the sampled pair changes. C++ then applies the transition table.

The current class requires the two GPIO numbers to be consecutive. All board phase pairs satisfy this requirement, including the reversed-order EN2 and EN6 pairs. `canonicalize()` restores caller-defined A/B meaning.

## Counts and detents

`EncoderPIO` stores raw quadrature transition count. `getDetents(4)` divides by four, which is appropriate only when one physical detent corresponds to four valid state transitions for the selected encoder and mechanical configuration.

The exact detent divisor should be a per-control configuration constant rather than an unexamined global assumption.

## No encoder acceleration

The current product requirement is no encoder acceleration. Parameter changes should derive from actual detent movement and configured step size, not rotation speed.

## Button debounce

`DebouncedButton` treats the switch as active low using `INPUT_PULLUP`.

State variables:

- `_lastRaw`: most recently sampled electrical level;
- `_stable`: accepted debounced level;
- `_lastChangeMs`: time the raw level last changed;
- `_pressStartMs`: accepted press start;
- one-shot pressed, released, and long-press edges.

The defaults are 15 ms debounce and 600 ms long press.

## Edge semantics

`pressed()`, `released()`, and `longPressed()` are one-update edges. They are cleared at the beginning of every `update()` call. Code must consume them during the same main-loop iteration.

`isPressed()` returns stable state rather than an edge.

## Current integration status

The encoder and button classes exist but the current integration sketches do not instantiate six devices, poll them, update the six parameters, or serialize button state. The hardware map and packet numbering are defined; the application wiring remains to be implemented.

## Coherent sampling requirement

A true coherent snapshot should copy all six encoder values and normalized button bitmaps into a temporary structure before emitting `SNAPSHOT_BEGIN`. Changes that occur during packet transmission then belong to a later event or snapshot, not the in-progress one.

Recommended capture structure:

```cpp
struct ControlSnapshot {
    uint16_t sequence;
    int32_t encoder_value[6];
    uint8_t pressed_bitmap;
    uint8_t long_bitmap;
    uint8_t status_flags;
};
```

That structure is an internal model only. It must still be serialized into explicit protocol bytes rather than transmitted directly.
