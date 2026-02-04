#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "ParamBinding.h"

/*****************************************************************************************
 *
 * CommandDispatcher
 * =================
 * Consumes validated RX packets from the master (SPI MOSI) and applies authoritative
 * actions to local parameter state. Any resulting state changes are emitted as events
 * and later published back to the master via StatePublisher.
 *
 *****************************************************************************************/

class CommandDispatcher {
public:
  CommandDispatcher(Param* params, uint8_t paramCount, EventQueue<16>& q);

  void handlePacket(const uint8_t pkt[8]);

private:
  Param* _params;
  uint8_t _paramCount;
  EventQueue<16>& _eventQueue;

  bool validate(const uint8_t pkt[8]) const;
};
