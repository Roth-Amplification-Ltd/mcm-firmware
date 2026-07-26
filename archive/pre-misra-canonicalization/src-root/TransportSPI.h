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
#include "HardwareConfig.h"

/**
 * @file TransportSPI.h
 * @brief Byte transport and CS framing for the MCM's SPI-only host interface.
 */

/**
 * @class TransportSPI
 * @brief PIO-backed SPI Mode-0 peripheral with one RX packet slot and a TX FIFO.
 *
 * Responsibilities:
 *
 * - configure GPIO and one RP2040 PIO state machine;
 * - receive MOSI bytes produced by the PIO program;
 * - accept a command only when exactly eight bytes were enclosed by CS;
 * - queue complete eight-byte response packets;
 * - feed queued bytes into the PIO TX FIFO;
 * - assert or deassert the active-high SLAVE_IRQ output.
 *
 * Non-responsibilities:
 *
 * - sync/version/CRC validation;
 * - command interpretation;
 * - encoder or button scanning;
 * - parameter ownership;
 * - snapshot completeness checks.
 *
 * Call service() on every fast main-loop pass. The current implementation
 * detects CS edges by polling digitalRead(), so excessive service latency can
 * cause software framing to miss a short transaction even though PIO shifted
 * the electrical data correctly.
 *
 * @warning TX queue entries are retired when bytes are handed to PIO, not when
 *          the master physically clocks them. SLAVE_IRQ therefore reflects
 *          software-queue occupancy rather than guaranteed on-wire completion.
 */
class TransportSPI {
public:
  TransportSPI();

  /** Configure GPIO, load the PIO program, initialize FIFOs, and enable the SM. */
  void begin();

  /** Drain RX FIFO, maintain CS framing, and fill TX FIFO. */
  void service();

  /**
   * Copy one complete packet into the software TX ring.
   * @return true when accepted; false when the ring is already full.
   */
  bool enqueueTxPacket(const uint8_t* packet);

  /** @return true when one complete CS-framed RX packet awaits consumption. */
  bool hasRxPacket() const;

  /**
   * Copy the completed RX packet and clear the ready flag.
   * @return false when no complete packet is available.
   */
  bool popRxPacket(uint8_t* packet);

private:
  // Aliases keep the transport implementation tied to the centralized map.
  static constexpr uint8_t PIN_SCK  = McmHardware::SPI_SCK_PIN;
  static constexpr uint8_t PIN_CS   = McmHardware::SPI_CS_PIN;
  static constexpr uint8_t PIN_MISO = McmHardware::SPI_MISO_PIN;
  static constexpr uint8_t PIN_MOSI = McmHardware::SPI_MOSI_PIN;
  static constexpr uint8_t PIN_IRQ  = McmHardware::SPI_IRQ_PIN;

  static constexpr bool IRQ_ACTIVE_HIGH = McmHardware::SPI_IRQ_ACTIVE_HIGH;
  static constexpr uint8_t PKT_LEN = McmHardware::SPI_PACKET_LENGTH;

  /** Number of complete response packets retained in software. */
  static constexpr uint8_t TX_Q_DEPTH = 8;

  // RP2040 PIO allocation. The constructor currently selects PIO1/SM2.
  PIO _pio;
  uint _sm;
  uint _offset;

  // Complete-packet TX ring. Head is the next insertion point; tail is the
  // packet currently being fed into PIO.
  uint8_t _txQ[TX_Q_DEPTH][PKT_LEN];
  uint8_t _txHead;
  uint8_t _txTail;
  uint8_t _txCount;
  uint8_t _txByteIndex;

  // Single received-packet staging slot.
  uint8_t _rxPkt[PKT_LEN];
  uint8_t _rxIndex;
  bool _rxReady;

  // Previous sampled CS state, used to synthesize software frame edges.
  bool _csWasLow;

  /** Drive the physical IRQ pin using its configured polarity. */
  void setIrqLevel(bool asserted);

  /** Left-align one byte in a 32-bit PIO TX word for MSB-first shifting. */
  void pushTxByte(uint8_t byte);

  /** Discard a partial RX frame and clear the ready flag. */
  void resetRxFrame();
};
