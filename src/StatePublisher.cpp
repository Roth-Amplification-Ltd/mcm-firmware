#include "StatePublisher.h"

static inline void enc24(int32_t v, uint8_t& b0, uint8_t& b1, uint8_t& b2) {
  b0 = v & 0xFF;
  b1 = (v >> 8) & 0xFF;
  b2 = (v >> 16) & 0xFF;
}

void StatePublisher::service() {
  if (_ready) return;
  Event ev;
  if (!_q.pop(ev)) return;

  _pkt[0] = PROTO_SYNC_BYTE;
  _pkt[1] = PROTO_VERSION;

  switch (ev.type) {
    case EVT_PARAM_CHANGED:
      _pkt[2] = MSG_PARAM_STATE;
      _pkt[3] = ev.index;
      enc24(ev.value, _pkt[4], _pkt[5], _pkt[6]);
      break;
    case EVT_SNAPSHOT_BEGIN:
      _pkt[2] = MSG_SNAPSHOT_BEGIN;
      _pkt[3] = ev.index;
      enc24(ev.value, _pkt[4], _pkt[5], _pkt[6]);
      break;
    case EVT_SNAPSHOT_END:
      _pkt[2] = MSG_SNAPSHOT_END;
      _pkt[3] = ev.index;
      enc24(ev.value, _pkt[4], _pkt[5], _pkt[6]);
      break;
    default:
      return;
  }

  _pkt[7] = proto_crc8(_pkt, 7);
  _ready = true;
}

bool StatePublisher::popPacket(uint8_t* out8) {
  if (!_ready) return false;
  memcpy(out8, _pkt, 8);
  _ready = false;
  return true;
}
