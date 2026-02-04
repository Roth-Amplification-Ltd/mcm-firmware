#include "CommandDispatcher.h"

CommandDispatcher::CommandDispatcher(Param* params,
                                     uint8_t paramCount,
                                     EventQueue<16>& q)
: _params(params), _paramCount(paramCount), _eventQueue(q) {}

bool CommandDispatcher::validate(const uint8_t pkt[8]) const {
  if (pkt[0] != PROTO_SYNC_BYTE) return false;
  if (pkt[1] != PROTO_VERSION)   return false;
  if (proto_crc8(pkt, 7) != pkt[7]) return false;
  return true;
}

void CommandDispatcher::handlePacket(const uint8_t pkt[8]) {
  if (!validate(pkt)) return;

  uint8_t type  = pkt[2];
  uint8_t index = pkt[3];

  switch (type) {

    case MSG_CMD_GET_SNAPSHOT:
      // Emit PARAM_STATE for every parameter
      for (uint8_t i = 0; i < _paramCount; i++) {
        Event e;
        e.type  = EVT_PARAM_CHANGED;
        e.index = i;
        e.value = _params[i].value;
        _eventQueue.push(e);
      }
      break;

    case MSG_CMD_RESET_PARAM:
      if (index < _paramCount) {
        _params[index].resetToDefault();
        Event e;
        e.type  = EVT_PARAM_CHANGED;
        e.index = index;
        e.value = _params[index].value;
        _eventQueue.push(e);
      }
      break;

    case MSG_CMD_RESET_ALL:
      for (uint8_t i = 0; i < _paramCount; i++) {
        _params[i].resetToDefault();
        Event e;
        e.type  = EVT_PARAM_CHANGED;
        e.index = i;
        e.value = _params[i].value;
        _eventQueue.push(e);
      }
      break;

    case MSG_CMD_SET_LED:
      // Stub: LED handling will be implemented in a later layer
      break;

    case MSG_CMD_GET_INFO:
      // Stub: version / ID response will be implemented later
      break;

    case MSG_CMD_NOP:
    default:
      break;
  }
}
