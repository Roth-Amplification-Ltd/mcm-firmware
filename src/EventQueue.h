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
  EventQueue() : h(0), t(0), c(0) {}
  bool push(const Event& e) {
    if (c >= SIZE) return false;
    buf[h] = e; h = (h + 1) % SIZE; c++; return true;
  }
  bool pop(Event& o) {
    if (c == 0) return false;
    o = buf[t]; t = (t + 1) % SIZE; c--; return true;
  }
private:
  Event buf[SIZE];
  uint8_t h, t, c;
};
