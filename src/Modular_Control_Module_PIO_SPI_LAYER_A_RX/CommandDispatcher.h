#pragma once
#include <Arduino.h>
#include "TransportProtocol.h"
#include "EventQueue.h"
#include "ParamBinding.h"
#include "SnapshotFlow.h"

/**
 * @class CommandDispatcher
 * @brief Validate host command packets and translate them into state changes.
 *
 * The dispatcher owns protocol semantics but not byte transport. It receives a
 * complete eight-byte packet from TransportSPI, rejects invalid envelope/CRC,
 * applies supported commands to the authoritative Param array, and emits
 * transport-independent events.
 */
class CommandDispatcher {
public:
  CommandDispatcher(Param* params,
                    uint8_t paramCount,
                    EventQueue<16>& queue,
                    SnapshotFlowState& flow)
  : _params(params),
    _paramCount(paramCount),
    _q(queue),
    _flow(flow)
  {}

  /** Validate and execute one complete master-to-MCM command packet. */
  void handlePacket(const uint8_t pkt[8]);

  /** Request BEGIN, one PARAM_STATE per parameter, and END. */
  void requestSnapshot();

private:
  Param* _params;
  uint8_t _paramCount;
  EventQueue<16>& _q;
  SnapshotFlowState& _flow;

  bool validate(const uint8_t pkt[8]) const;
  void enqueueSnapshotFramed();
};
