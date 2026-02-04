#pragma once
#include <Arduino.h>

static const uint8_t PROTO_SYNC_BYTE = 0xA5;
static const uint8_t PROTO_VERSION   = 0x01;

enum ProtoMsgType : uint8_t {
  MSG_PARAM_STATE   = 0x01,
  MSG_BUTTON_EVENT  = 0x02,
  MSG_HEARTBEAT     = 0x7F
};

static inline uint8_t proto_crc8(const uint8_t* data, uint8_t len)
{
  uint8_t crc = 0x00;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
    }
  }
  return crc;
}
