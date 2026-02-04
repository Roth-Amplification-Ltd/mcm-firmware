#include "StatePublisher.h"

void StatePublisher::service() {
  if (_ready) return;
  Event ev;
  if (!_q.pop(ev)) return;
  if (ev.type == EVT_PARAM_CHANGED) {
    _pkt[0] = PROTO_SYNC_BYTE;
    _pkt[1] = PROTO_VERSION;
    _pkt[2] = MSG_PARAM_STATE;
    _pkt[3] = ev.index;
    _pkt[4] = (uint8_t)(ev.value & 0xFF);
    _pkt[5] = (uint8_t)((ev.value >> 8) & 0xFF);
    _pkt[6] = (uint8_t)((ev.value >> 16) & 0xFF);
    _pkt[7] = proto_crc8(_pkt, 7);
    _ready = true;
  }
}

bool StatePublisher::popPacket(uint8_t* out8) {
  if (!_ready) return false;
  memcpy(out8, _pkt, 8);
  _ready = false;
  return true;
}
