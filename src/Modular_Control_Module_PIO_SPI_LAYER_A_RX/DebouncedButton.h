#pragma once
#include <Arduino.h>

class DebouncedButton {
public:
  DebouncedButton() : _pin(255), _stable(1), _lastRaw(1), _lastChangeMs(0),
                      _debounceMs(15), _pressStartMs(0), _longPressMs(600),
                      _pressedEdge(false), _releasedEdge(false), _longEdge(false) {}

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

  void update() {
    _pressedEdge = _releasedEdge = _longEdge = false;
    int raw = digitalRead(_pin);
    uint32_t now = millis();
    if (raw != _lastRaw) {
      _lastRaw = raw;
      _lastChangeMs = now;
    }
    if ((now - _lastChangeMs) >= _debounceMs) {
      if (_stable != _lastRaw) {
        int prev = _stable;
        _stable = _lastRaw;
        if (prev == 1 && _stable == 0) {
          _pressedEdge = true;
          _pressStartMs = now;
        } else if (prev == 0 && _stable == 1) {
          _releasedEdge = true;
          _pressStartMs = 0;
        }
      }
    }
    if (_stable == 0 && _pressStartMs != 0) {
      if ((now - _pressStartMs) >= _longPressMs) {
        _longEdge = true;
        _pressStartMs = 0;
      }
    }
  }

  bool isPressed() const { return _stable == 0; }
  bool pressed() const { return _pressedEdge; }
  bool released() const { return _releasedEdge; }
  bool longPressed() const { return _longEdge; }

private:
  uint8_t  _pin;
  int      _stable;
  int      _lastRaw;
  uint32_t _lastChangeMs;
  uint16_t _debounceMs;
  uint32_t _pressStartMs;
  uint16_t _longPressMs;
  bool _pressedEdge, _releasedEdge, _longEdge;
};
