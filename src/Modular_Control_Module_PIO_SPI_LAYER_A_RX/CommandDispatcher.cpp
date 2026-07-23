#include "CommandDispatcher.h"

bool CommandDispatcher::validate(const uint8_t pkt[8]) const {
  if (pkt[0] != PROTO_SYNC_BYTE) return false;
  if (pkt[1] != PROTO_VERSION)   return false;
  if (proto_crc8(pkt, 7) != pkt[7]) return false;
  return true;
}

void CommandDispatcher::enqueueSnapshotFramed() {
  // Flow control: do not overlap snapshots.
  if (_flow.snapshot_in_progress) {
    // Snapshot already queued/serializing. Ignore further requests.
    return;
  }

  _flow.snapshot_in_progress = true;
  _flow.seq_id++; // reserved for future use (not transmitted yet)

  _q.push({EVT_SNAPSHOT_BEGIN, _paramCount, 0});

  for (uint8_t i = 0; i < _paramCount; i++) {
    _q.push({EVT_PARAM_CHANGED, i, _params[i].value});
  }

  _q.push({EVT_SNAPSHOT_END, _paramCount, 0});
}

void CommandDispatcher::requestSnapshot() {
  enqueueSnapshotFramed();
}

void CommandDispatcher::handlePacket(const uint8_t pkt[8]) {
  if (!validate(pkt)) return;

  switch (pkt[2]) {

    case MSG_CMD_GET_SNAPSHOT:
      enqueueSnapshotFramed();
      break;

    case MSG_CMD_RESET_PARAM:
      if (pkt[3] < _paramCount) {
        _params[pkt[3]].resetToDefault();
        _q.push({EVT_PARAM_CHANGED, pkt[3], _params[pkt[3]].value});
      }
      break;

    case MSG_CMD_RESET_ALL:
      for (uint8_t i = 0; i < _paramCount; i++) {
        _params[i].resetToDefault();
        _q.push({EVT_PARAM_CHANGED, i, _params[i].value});
      }
      break;

    case MSG_CMD_NOP:
    default:
      // RESYNC is handled in the main loop because it must also clear the
      // SPI TX queue (transport concern). Keeping that logic in the .ino
      // preserves the clean layering: transport clears transport, dispatcher
      // clears/creates events.
      break;
  }
}
