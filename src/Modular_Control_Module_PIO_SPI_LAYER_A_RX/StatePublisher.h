#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "SnapshotFlow.h"

class StatePublisher {
public:
  StatePublisher(EventQueue<16>& q, SnapshotFlowState& flow) : _q(q), _flow(flow), _ready(false) {}
  void service();
  bool hasPacket() const { return _ready; }
  bool popPacket(uint8_t* out8);
private:
  EventQueue<16>& _q;
  SnapshotFlowState& _flow;
  bool _ready;
  uint8_t _pkt[8];
};
