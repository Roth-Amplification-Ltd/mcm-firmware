#pragma once
#include <Arduino.h>

static const uint8_t PROTO_SYNC_BYTE = 0xA5;
static const uint8_t PROTO_VERSION   = 0x01;

enum ProtoMsgType : uint8_t {
  // Slave -> Master
  MSG_PARAM_STATE      = 0x01,
  MSG_BUTTON_EVENT     = 0x02,
  MSG_HEARTBEAT        = 0x7F,

  // Master -> Slave commands (Layer A)
  MSG_CMD_NOP          = 0x80,
  MSG_CMD_GET_SNAPSHOT = 0x81,
  MSG_CMD_RESET_PARAM  = 0x82,
  MSG_CMD_RESET_ALL    = 0x83,
  MSG_CMD_SET_LED      = 0x84,
  MSG_CMD_GET_INFO     = 0x85
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
