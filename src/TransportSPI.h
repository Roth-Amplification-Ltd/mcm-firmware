#pragma once
#include <Arduino.h>
#include "hardware/pio.h"

/*****************************************************************************************
 *
 * TransportSPI
 * ============
 * PIO-based SPI SLAVE transport (Mode 0, MSB-first) with:
 *   - TX: MCM -> Master (MISO)
 *   - RX: Master -> MCM (MOSI)
 *   - CS-based RX framing (robust packet assembly)
 *   - IRQ output pin (MCM -> Master) asserted HIGH when TX data is pending
 *
 * WHY IRQ?
 * --------
 * The master should not poll SPI continuously. Instead:
 *   1) MCM detects state change -> enqueues TX packet(s)
 *   2) MCM asserts IRQ pin HIGH ("data available")
 *   3) Master ISR/task sees IRQ -> clocks exactly 8 bytes per transaction
 *   4) MCM deasserts IRQ when TX queue becomes empty
 *
 * IMPORTANT: IRQ is LEVEL-BASED (not a pulse)
 * -------------------------------------------
 * Level-based signaling is far more robust because:
 *   - The master cannot "miss" the interrupt if it is busy
 *   - IRQ naturally represents "data pending" rather than "event occurred"
 *
 *****************************************************************************************/

class TransportSPI {
public:
  TransportSPI();

  void begin();
  void service();

  // ------------------------------- TX (MISO) -------------------------------
  // Enqueue a single 8-byte packet for transmission.
  // Returns false if the TX queue is full.
  bool enqueueTxPacket(const uint8_t* pkt8);

  // ------------------------------- RX (MOSI) -------------------------------
  // True if an entire, CS-framed 8-byte command packet has been received.
  bool hasRxPacket() const;

  // Copy the RX packet out and clear the ready flag.
  bool popRxPacket(uint8_t* pkt8);

private:
  // Fixed SPI pins (must match SpiSlaveM0.pio)
  static const uint8_t PIN_SCK  = 12;
  static const uint8_t PIN_CS   = 13;
  static const uint8_t PIN_MISO = 14;
  static const uint8_t PIN_MOSI = 15;

  // Interrupt pin (MCM -> Master)
  static const uint8_t PIN_IRQ  = 11;   // GPIO11 (Pin 14) — user-specified
  static const bool    IRQ_ACTIVE_HIGH = true;

  static const uint8_t PKT_LEN = 8;

  // TX packet queue depth (tune later if needed)
  static const uint8_t TX_Q_DEPTH = 8;

  PIO  _pio;
  uint _sm;
  uint _offset;

  // ------------------------------- TX queue -------------------------------
  uint8_t _txQ[TX_Q_DEPTH][PKT_LEN];
  uint8_t _txHead;
  uint8_t _txTail;
  uint8_t _txCount;

  // The next byte index in the *current head packet* we are streaming to PIO.
  uint8_t _txByteIndex;

  // ------------------------------- RX state -------------------------------
  uint8_t _rxPkt[PKT_LEN];
  uint8_t _rxIndex;
  bool    _rxReady;

  // CS tracking to make RX framing robust
  bool    _csWasLow;

  // Helpers
  void setIrqLevel(bool asserted);
  void pushTxByte(uint8_t b);
  void resetRxFrame();
};
