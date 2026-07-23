#pragma once
#include <Arduino.h>
#include "hardware/pio.h"
#include "HardwareConfig.h"

/*
 * PIO-based SPI Mode 0 peripheral transport.
 *
 * This is the sole external serial control interface on the current MCM PCB.
 * The MCM receives commands on MOSI, returns state packets on MISO, and raises
 * the level-based SLAVE_IRQ line while response data is pending.
 */
class TransportSPI {
public:
  TransportSPI();
  void begin();
  void service();
  bool enqueueTxPacket(const uint8_t* packet);
  bool hasRxPacket() const;
  bool popRxPacket(uint8_t* packet);

private:
  static constexpr uint8_t PIN_SCK  = McmHardware::SPI_SCK_PIN;
  static constexpr uint8_t PIN_CS   = McmHardware::SPI_CS_PIN;
  static constexpr uint8_t PIN_MISO = McmHardware::SPI_MISO_PIN;
  static constexpr uint8_t PIN_MOSI = McmHardware::SPI_MOSI_PIN;
  static constexpr uint8_t PIN_IRQ  = McmHardware::SPI_IRQ_PIN;
  static constexpr bool IRQ_ACTIVE_HIGH = McmHardware::SPI_IRQ_ACTIVE_HIGH;
  static constexpr uint8_t PKT_LEN = McmHardware::SPI_PACKET_LENGTH;
  static constexpr uint8_t TX_Q_DEPTH = 8;

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
  void pushTxByte(uint8_t byte);
  void resetRxFrame();
};
