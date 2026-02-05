#pragma once
#include <Arduino.h>
#include "hardware/pio.h"

/*****************************************************************************************
 *
 * TransportSPI
 * ============
 * PIO-based SPI slave with RX and TX support.
 *
 * Responsibilities:
 *   - Shift out 8-byte response packets on MISO
 *   - Capture 8-byte command packets from MOSI
 *   - Enforce CS-framed transactions
 *
 *****************************************************************************************/

class TransportSPI {
public:
  TransportSPI();

  void begin();
  void service();

  // TX
  void loadTxPacket(const uint8_t* pkt8);

  // RX
  bool hasRxPacket() const;
  bool popRxPacket(uint8_t* pkt8);

private:
  static const uint8_t PIN_SCK  = 12;
  static const uint8_t PIN_CS   = 13;
  static const uint8_t PIN_MISO = 14;
  static const uint8_t PIN_MOSI = 15;

  static const uint8_t PKT_LEN = 8;

  PIO  _pio;
  uint _sm;
  uint _offset;

  // TX state
  uint8_t _txPkt[PKT_LEN];
  bool    _txValid;
  uint8_t _txIndex;

  // RX state
  uint8_t _rxPkt[PKT_LEN];
  uint8_t _rxIndex;
  bool    _rxReady;

  void pushTxByte(uint8_t b);
};
