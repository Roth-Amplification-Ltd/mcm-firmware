
#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"

class StatePublisher {
public:
  StatePublisher(EventQueue<16>& q) : _q(q), _ready(false) {}
  void service();
  bool hasPacket() const { return _ready; }
  bool popPacket(uint8_t* out);
private:
  EventQueue<16>& _q;
  bool _ready;
  uint8_t _pkt[8];
};
