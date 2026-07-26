/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "TransportSPI.h"
#include "SpiSlaveM0.pio.h"

TransportSPI::TransportSPI()
: _pio(pio1), _sm(2), _offset(0),
  _txHead(0), _txTail(0), _txCount(0), _txByteIndex(0),
  _rxIndex(0), _rxReady(false),
  _csWasLow(false)
{}

void TransportSPI::begin()
{
  pinMode(PIN_SCK, INPUT);
  pinMode(PIN_CS, INPUT_PULLUP);
  pinMode(PIN_MOSI, INPUT);

  pinMode(PIN_IRQ, OUTPUT);
  setIrqLevel(false);

  _offset = pio_add_program(_pio, &spi_slave_m0_msb_rx_program);

  pio_gpio_init(_pio, PIN_MISO);
  pio_sm_set_consecutive_pindirs(_pio, _sm, PIN_MISO, 1, true);

  pio_sm_config c = spi_slave_m0_msb_rx_program_get_default_config(_offset);
  sm_config_set_in_pins(&c, PIN_MOSI);
  sm_config_set_out_pins(&c, PIN_MISO, 1);

  // Historical copy does not configure sm_config_set_jmp_pin(). This is one
  // reason the corrected root transport/PIO pair is authoritative.
  sm_config_set_out_shift(&c, false, false, 32);
  sm_config_set_in_shift(&c, true, false, 32);
  sm_config_set_clkdiv(&c, 1.0f);

  pio_sm_init(_pio, _sm, _offset, &c);
  pio_sm_set_enabled(_pio, _sm, true);
  pio_sm_clear_fifos(_pio, _sm);
  resetRxFrame();
}

bool TransportSPI::enqueueTxPacket(const uint8_t* pkt8)
{
  if (_txCount >= TX_Q_DEPTH) return false;

  memcpy(_txQ[_txHead], pkt8, PKT_LEN);
  _txHead = (_txHead + 1) % TX_Q_DEPTH;
  _txCount++;
  setIrqLevel(true);
  return true;
}

bool TransportSPI::hasRxPacket() const
{
  return _rxReady;
}

bool TransportSPI::popRxPacket(uint8_t* pkt8)
{
  if (!_rxReady) return false;

  memcpy(pkt8, _rxPkt, PKT_LEN);
  _rxReady = false;
  return true;
}

void TransportSPI::service()
{
  // CS is the software frame authority. Exactly eight bytes between falling
  // and rising edges become one command packet.
  const bool csLow = (digitalRead(PIN_CS) == LOW);

  if (csLow && !_csWasLow) {
    resetRxFrame();
    _csWasLow = true;
  } else if (!csLow && _csWasLow) {
    _rxReady = (_rxIndex == PKT_LEN);
    _rxIndex = 0;
    _csWasLow = false;
  }

  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    const uint32_t word = pio_sm_get(_pio, _sm);
    const uint8_t byte = (uint8_t)((word >> 24) & 0xFF);

    if (csLow && _rxIndex < PKT_LEN) {
      _rxPkt[_rxIndex++] = byte;
    }
  }

  // Queue entries are consumed when bytes enter PIO FIFO, not when they are
  // physically shifted. IRQ therefore has the early-deassertion limitation
  // described in docs/KNOWN_LIMITATIONS.md.
  while (_txCount > 0 && !pio_sm_is_tx_fifo_full(_pio, _sm)) {
    pushTxByte(_txQ[_txTail][_txByteIndex]);
    _txByteIndex++;

    if (_txByteIndex >= PKT_LEN) {
      _txByteIndex = 0;
      _txTail = (_txTail + 1) % TX_Q_DEPTH;
      _txCount--;

      if (_txCount == 0) setIrqLevel(false);
    }
  }
}

void TransportSPI::setIrqLevel(bool asserted)
{
  if (IRQ_ACTIVE_HIGH) {
    digitalWrite(PIN_IRQ, asserted ? HIGH : LOW);
  } else {
    digitalWrite(PIN_IRQ, asserted ? LOW : HIGH);
  }
}

void TransportSPI::pushTxByte(uint8_t b)
{
  pio_sm_put(_pio, _sm, ((uint32_t)b) << 24);
}

void TransportSPI::clearTxQueue()
{
  // This clears only software bookkeeping. It intentionally preserves current
  // behavior and does not flush already-loaded PIO TX FIFO words.
  _txHead = 0;
  _txTail = 0;
  _txCount = 0;
  _txByteIndex = 0;
  setIrqLevel(false);
}

void TransportSPI::resetRxFrame()
{
  _rxIndex = 0;
  _rxReady = false;
}
