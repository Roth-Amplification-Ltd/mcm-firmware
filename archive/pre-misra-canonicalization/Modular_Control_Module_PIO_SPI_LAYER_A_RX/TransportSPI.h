/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <Arduino.h>
#include "hardware/pio.h"

/**
 * @file TransportSPI.h
 * @brief Historical integrated SPI transport with RESYNC queue clearing.
 *
 * @warning This is not the preferred hardware-aligned copy. The corrected root
 *          `src/TransportSPI.*` centralizes pins in HardwareConfig.h and pairs
 *          with the corrected root `src/SpiSlaveM0.pio`.
 */
class TransportSPI {
public:
  TransportSPI();

  void begin();
  void service();

  /** Enqueue one complete eight-byte MISO response. */
  bool enqueueTxPacket(const uint8_t* pkt8);

  /**
   * Clear the software TX ring and deassert IRQ.
   * Already-loaded PIO FIFO bytes remain and may still be clocked by the host.
   */
  void clearTxQueue();

  bool hasRxPacket() const;
  bool popRxPacket(uint8_t* pkt8);

private:
  // Historical hard-coded copy of the KiCad-verified host pin map.
  static const uint8_t PIN_SCK  = 12;
  static const uint8_t PIN_CS   = 13;
  static const uint8_t PIN_MISO = 14;
  static const uint8_t PIN_MOSI = 15;
  static const uint8_t PIN_IRQ  = 11;
  static const bool IRQ_ACTIVE_HIGH = true;

  static const uint8_t PKT_LEN = 8;
  static const uint8_t TX_Q_DEPTH = 8;

  PIO _pio;
  uint _sm;
  uint _offset;

  uint8_t _txQ[TX_Q_DEPTH][PKT_LEN];
  uint8_t _txHead;
  uint8_t _txTail;
  uint8_t _txCount;
  uint8_t _txByteIndex;

  uint8_t _rxPkt[PKT_LEN];
  uint8_t _rxIndex;
  bool _rxReady;
  bool _csWasLow;

  void setIrqLevel(bool asserted);
  void pushTxByte(uint8_t b);
  void resetRxFrame();
};
