#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "ParamBinding.h"

class CommandDispatcher {
public:
  CommandDispatcher(Param* params, uint8_t paramCount, EventQueue<16>& q)
  : _params(params), _paramCount(paramCount), _q(q) {}
  void handlePacket(const uint8_t pkt[8]);
private:
  Param* _params;
  uint8_t _paramCount;
  EventQueue<16>& _q;
  bool validate(const uint8_t pkt[8]) const;
  void enqueueSnapshot();
};
