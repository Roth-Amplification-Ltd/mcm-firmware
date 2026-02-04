
#pragma once
#include <Arduino.h>

enum EventType : uint8_t {
  EVT_NONE = 0,
  EVT_PARAM_CHANGED,
  EVT_BUTTON_PRESSED,
  EVT_BUTTON_LONG
};

struct Event {
  EventType type;
  uint8_t index;
  int32_t value;
};

template <uint8_t SIZE>
class EventQueue {
public:
  EventQueue() : _head(0), _tail(0), _count(0) {}
  bool push(const Event& e) {
    if (_count >= SIZE) return false;
    _buf[_head] = e;
    _head = (_head + 1) % SIZE;
    _count++;
    return true;
  }
  bool pop(Event& out) {
    if (_count == 0) return false;
    out = _buf[_tail];
    _tail = (_tail + 1) % SIZE;
    _count--;
    return true;
  }
private:
  Event _buf[SIZE];
  uint8_t _head, _tail, _count;
};
