#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "ParamBinding.h"
#include "SnapshotFlow.h"

/*****************************************************************************************
 *
 * CommandDispatcher (Layer D)
 * ==========================
 * Consumes validated command packets from the master and produces internal events.
 *
 * New in Layer D:
 *   - snapshot flow-control (avoid overlapping snapshots)
 *   - RESYNC command is handled at a higher level (main loop) because it also
 *     clears the SPI TX queue; see .ino for exact semantics.
 *
 *****************************************************************************************/

class CommandDispatcher {
public:
  CommandDispatcher(Param* params, uint8_t paramCount, EventQueue<16>& q, SnapshotFlowState& flow)
  : _params(params), _paramCount(paramCount), _q(q), _flow(flow) {}

  void handlePacket(const uint8_t pkt[8]);

  // Force enqueue of a framed snapshot sequence (BEGIN, PARAM_STATE×N, END).
  // Used by RESYNC handling in the main loop.
  void requestSnapshot();

private:
  Param* _params;
  uint8_t _paramCount;
  EventQueue<16>& _q;
  SnapshotFlowState& _flow;

  bool validate(const uint8_t pkt[8]) const;
  void enqueueSnapshotFramed();
};
