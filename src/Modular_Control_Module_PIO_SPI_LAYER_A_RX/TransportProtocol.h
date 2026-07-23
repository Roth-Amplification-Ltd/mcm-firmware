#pragma once
#include <Arduino.h>

// -------------------- Fixed packet format --------------------
// [0] SYNC     = 0xA5
// [1] VERSION  = 0x01
// [2] TYPE
// [3] INDEX
// [4] VALUE0 (LSB)
// [5] VALUE1
// [6] VALUE2 (MSB of 24-bit payload)
// [7] CRC8 over bytes [0..6]
static const uint8_t PROTO_SYNC_BYTE = 0xA5;
static const uint8_t PROTO_VERSION   = 0x01;

enum ProtoMsgType : uint8_t {
  // ---------------- Slave -> Master state messages ----------------
  MSG_PARAM_STATE     = 0x01,
  MSG_SNAPSHOT_BEGIN  = 0x10,
  MSG_SNAPSHOT_END    = 0x11,

  // ---------------- Master -> Slave commands ----------------
  MSG_CMD_NOP          = 0x80,
  MSG_CMD_GET_SNAPSHOT = 0x81,
  MSG_CMD_RESET_PARAM  = 0x82,
  MSG_CMD_RESET_ALL    = 0x83,
  MSG_CMD_SET_LED      = 0x84, // reserved/stub
  MSG_CMD_GET_INFO     = 0x85, // reserved/stub

  // Layer D: robust resynchronization command
  // ----------------------------------------
  // Semantics:
  //   "Discard pending outbound state (TX queue), discard pending internal events,
  //    and immediately start a fresh framed snapshot (BEGIN, PARAM_STATE×N, END)."
  //
  // This is the master’s "escape hatch" for any error condition:
  //   - master rebooted mid-stream
  //   - CRC errors / framing confusion
  //   - missed snapshot end
  //   - hot-plug recovery
  MSG_CMD_RESYNC       = 0x86
};

static inline uint8_t proto_crc8(const uint8_t* d, uint8_t n) {
  uint8_t c = 0;
  for (uint8_t i = 0; i < n; i++) {
    c ^= d[i];
    for (uint8_t b = 0; b < 8; b++) {
      c = (c & 0x80) ? (uint8_t)((c << 1) ^ 0x07) : (uint8_t)(c << 1);
    }
  }
  return c;
}
