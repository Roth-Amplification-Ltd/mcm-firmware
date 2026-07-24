#pragma once
#include <Arduino.h>

/**
 * @file TransportProtocol.h
 * @brief Eight-byte MCM packet envelope, message IDs, and CRC-8 helper.
 *
 * This file belongs to the historical integrated Layer-A/Layer-D source tree.
 * Its message definitions are the most complete protocol constants currently
 * present on main, but they are not all wired into the newer root sketch.
 */

// Packet bytes:
//   [0] SYNC     = 0xA5
//   [1] VERSION  = 0x01
//   [2] TYPE     = command or response ID
//   [3] INDEX    = parameter/control index or type-specific count
//   [4] DATA0    = least-significant payload byte
//   [5] DATA1    = payload byte
//   [6] DATA2    = most-significant payload byte
//   [7] CRC8     = CRC over bytes 0 through 6
static const uint8_t PROTO_SYNC_BYTE = 0xA5;
static const uint8_t PROTO_VERSION   = 0x01;

enum ProtoMsgType : uint8_t {
  // MCM -> master state messages.
  MSG_PARAM_STATE     = 0x01,
  MSG_SNAPSHOT_BEGIN  = 0x10,
  MSG_SNAPSHOT_END    = 0x11,

  // Master -> MCM commands.
  MSG_CMD_NOP          = 0x80,
  MSG_CMD_GET_SNAPSHOT = 0x81,
  MSG_CMD_RESET_PARAM  = 0x82,
  MSG_CMD_RESET_ALL    = 0x83,
  MSG_CMD_SET_LED      = 0x84, // reserved; no implementation on main
  MSG_CMD_GET_INFO     = 0x85, // reserved; no implementation on main

  /**
   * Recovery command used when the host loses stream synchronization.
   *
   * The integration sketch clears pending software events and TX packets, then
   * requests a fresh framed snapshot. Bytes already inside PIO TX FIFO are not
   * cleared by the current implementation.
   */
  MSG_CMD_RESYNC       = 0x86
};

/**
 * Compute CRC-8 with polynomial 0x07, initial value 0x00, no reflection, and
 * no final XOR.
 *
 * @param d Pointer to bytes.
 * @param n Number of bytes; packet validation passes 7.
 */
static inline uint8_t proto_crc8(const uint8_t* d, uint8_t n) {
  uint8_t c = 0;

  for (uint8_t i = 0; i < n; i++) {
    c ^= d[i];

    for (uint8_t b = 0; b < 8; b++) {
      c = (c & 0x80)
        ? (uint8_t)((c << 1) ^ 0x07)
        : (uint8_t)(c << 1);
    }
  }

  return c;
}
