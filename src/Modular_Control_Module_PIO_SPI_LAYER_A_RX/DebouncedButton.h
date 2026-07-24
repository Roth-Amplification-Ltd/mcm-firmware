#pragma once
#include <Arduino.h>

/**
 * @class DebouncedButton
 * @brief Active-low button debouncer with press, release, and long-press edges.
 *
 * Electrical model:
 * - input uses INPUT_PULLUP;
 * - HIGH means released;
 * - LOW means pressed.
 *
 * Edge methods are one-update pulses. update() clears all edge flags at its
 * beginning, so application code must consume pressed(), released(), and
 * longPressed() during the same loop pass.
 */
class DebouncedButton {
public:
  DebouncedButton()
  : _pin(255),
    _stable(1),
    _lastRaw(1),
    _lastChangeMs(0),
    _debounceMs(15),
    _pressStartMs(0),
    _longPressMs(600),
    _pressedEdge(false),
    _releasedEdge(false),
    _longEdge(false)
  {}

  /**
   * Configure the input and capture its initial state.
   * @param pin Active-low switch GPIO.
   * @param debounceMs Raw level must remain unchanged for this long.
   * @param longPressMs Stable press duration required for longPressed().
   */
  void begin(uint8_t pin, uint16_t debounceMs = 15, uint16_t longPressMs = 600) {
    _pin = pin;
    _debounceMs = debounceMs;
    _longPressMs = longPressMs;

    pinMode(_pin, INPUT_PULLUP);

    _stable = digitalRead(_pin);
    _lastRaw = _stable;
    _lastChangeMs = millis();
    _pressStartMs = 0;
    _pressedEdge = _releasedEdge = _longEdge = false;
  }

  /** Sample the pin and update debounced state and one-shot edges. */
  void update() {
    // Edges are valid for one call interval only.
    _pressedEdge = _releasedEdge = _longEdge = false;

    const int raw = digitalRead(_pin);
    const uint32_t now = millis();

    // Any raw transition restarts the quiet-time debounce interval.
    if (raw != _lastRaw) {
      _lastRaw = raw;
      _lastChangeMs = now;
    }

    // Accept the new level only after it remained unchanged long enough.
    if ((now - _lastChangeMs) >= _debounceMs) {
      if (_stable != _lastRaw) {
        const int previous = _stable;
        _stable = _lastRaw;

        if (previous == HIGH && _stable == LOW) {
          _pressedEdge = true;
          _pressStartMs = now;
        } else if (previous == LOW && _stable == HIGH) {
          _releasedEdge = true;
          _pressStartMs = 0;
        }
      }
    }

    // Emit one long-press edge per accepted press. Clearing _pressStartMs after
    // emission suppresses repeated long edges while the button remains held.
    if (_stable == LOW && _pressStartMs != 0) {
      if ((now - _pressStartMs) >= _longPressMs) {
        _longEdge = true;
        _pressStartMs = 0;
      }
    }
  }

  bool isPressed() const { return _stable == LOW; }
  bool pressed() const { return _pressedEdge; }
  bool released() const { return _releasedEdge; }
  bool longPressed() const { return _longEdge; }

private:
  uint8_t _pin;
  int _stable;
  int _lastRaw;
  uint32_t _lastChangeMs;
  uint16_t _debounceMs;
  uint32_t _pressStartMs;
  uint16_t _longPressMs;
  bool _pressedEdge;
  bool _releasedEdge;
  bool _longEdge;
};
