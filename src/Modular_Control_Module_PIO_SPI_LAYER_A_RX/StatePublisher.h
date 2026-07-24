#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "SnapshotFlow.h"

/**
 * @class StatePublisher
 * @brief Serialize one internal event at a time into one wire packet.
 *
 * The publisher has one packet staging slot. service() does nothing while that
 * slot is occupied. The integration loop must call popPacket() and enqueue the
 * result into TransportSPI before the next event can be serialized.
 */
class StatePublisher {
public:
  StatePublisher(EventQueue<16>& queue, SnapshotFlowState& flow)
  : _q(queue), _flow(flow), _ready(false) {}

  /** Pop one event, serialize it, and compute CRC when the staging slot is free. */
  void service();

  bool hasPacket() const { return _ready; }

  /** Copy the staged packet and make the publisher ready for the next event. */
  bool popPacket(uint8_t* out8);

private:
  EventQueue<16>& _q;
  SnapshotFlowState& _flow;
  bool _ready;
  uint8_t _pkt[8];
};
