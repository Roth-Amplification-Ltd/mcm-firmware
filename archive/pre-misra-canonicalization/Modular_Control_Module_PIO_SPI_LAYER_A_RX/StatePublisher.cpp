/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "StatePublisher.h"

/** Encode the low 24 bits of an int32_t in little-endian byte order. */
static inline void enc24(int32_t v,
                         uint8_t& b0,
                         uint8_t& b1,
                         uint8_t& b2) {
  b0 = (uint8_t)(v & 0xFF);
  b1 = (uint8_t)((v >> 8) & 0xFF);
  b2 = (uint8_t)((v >> 16) & 0xFF);
}

void StatePublisher::service() {
  // Back-pressure boundary: never overwrite a packet the main loop has not yet
  // transferred into the transport queue.
  if (_ready) return;

  Event event;
  if (!_q.pop(event)) return;

  _pkt[0] = PROTO_SYNC_BYTE;
  _pkt[1] = PROTO_VERSION;

  switch (event.type) {
    case EVT_PARAM_CHANGED:
      _pkt[2] = MSG_PARAM_STATE;
      _pkt[3] = event.index;
      enc24(event.value, _pkt[4], _pkt[5], _pkt[6]);
      break;

    case EVT_SNAPSHOT_BEGIN:
      _pkt[2] = MSG_SNAPSHOT_BEGIN;
      _pkt[3] = event.index; // current code uses parameter count
      enc24(event.value, _pkt[4], _pkt[5], _pkt[6]); // reserved/currently zero
      break;

    case EVT_SNAPSHOT_END:
      _pkt[2] = MSG_SNAPSHOT_END;
      _pkt[3] = event.index; // current code uses parameter count
      enc24(event.value, _pkt[4], _pkt[5], _pkt[6]); // reserved/currently zero

      // Release the software overlap guard once END has entered the serialized
      // stream. This is earlier than physical on-wire completion.
      _flow.snapshot_in_progress = false;
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
