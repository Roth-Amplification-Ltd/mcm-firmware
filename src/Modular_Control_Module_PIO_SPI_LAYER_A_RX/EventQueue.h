#pragma once
#include <Arduino.h>

enum EventType : uint8_t {
  EVT_NONE = 0,
  EVT_PARAM_CHANGED,
  EVT_SNAPSHOT_BEGIN,
  EVT_SNAPSHOT_END
};

struct Event {
  EventType type;
  uint8_t index;
  int32_t value;
};

template <uint8_t N>
class EventQueue {
public:
  EventQueue() : h(0), t(0), c(0) {}

  bool push(const Event& e) {
    if (c >= N) return false;
    b[h] = e;
    h = (h + 1) % N;
    c++;
    return true;
  }

  bool pop(Event& o) {
    if (c == 0) return false;
    o = b[t];
    t = (t + 1) % N;
    c--;
    return true;
  }

  // Layer D helper: drop all pending events (used during RESYNC).
  void clear() {
    h = 0; t = 0; c = 0;
  }

  bool empty() const { return c == 0; }
  uint8_t count() const { return c; }

private:
  Event b[N];
  uint8_t h, t, c;
};
