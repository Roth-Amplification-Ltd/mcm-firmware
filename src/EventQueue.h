#pragma once
#include <Arduino.h>
enum EventType : uint8_t { EVT_NONE=0, EVT_PARAM_CHANGED };
struct Event { EventType type; uint8_t index; int32_t value; };
template <uint8_t N> class EventQueue {
public:
  EventQueue():h(0),t(0),c(0) {}
  bool push(const Event&e){ if(c>=N) return false; b[h]=e; h=(h+1)%N; c++; return true; }
  bool pop(Event&o){ if(!c) return false; o=b[t]; t=(t+1)%N; c--; return true; }
private: Event b[N]; uint8_t h,t,c;
};
