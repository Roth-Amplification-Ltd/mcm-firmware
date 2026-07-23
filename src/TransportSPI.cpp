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

  pio_sm_config config = spi_slave_m0_msb_rx_program_get_default_config(_offset);
  sm_config_set_in_pins(&config, PIN_MOSI);
  sm_config_set_out_pins(&config, PIN_MISO, 1);
  sm_config_set_jmp_pin(&config, PIN_CS);
  sm_config_set_out_shift(&config, false, false, 32);
  sm_config_set_in_shift(&config, true, false, 32);
  sm_config_set_clkdiv(&config, 1.0f);

  pio_sm_init(_pio, _sm, _offset, &config);
  pio_sm_clear_fifos(_pio, _sm);
  pio_sm_set_enabled(_pio, _sm, true);
  resetRxFrame();
}

bool TransportSPI::enqueueTxPacket(const uint8_t* packet)
{
  if (_txCount >= TX_Q_DEPTH) return false;
  memcpy(_txQ[_txHead], packet, PKT_LEN);
  _txHead = (_txHead + 1) % TX_Q_DEPTH;
  _txCount++;
  setIrqLevel(true);
  return true;
}

bool TransportSPI::hasRxPacket() const
{
  return _rxReady;
}

bool TransportSPI::popRxPacket(uint8_t* packet)
{
  if (!_rxReady) return false;
  memcpy(packet, _rxPkt, PKT_LEN);
  _rxReady = false;
  return true;
}

void TransportSPI::service()
{
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
    const uint8_t byte = static_cast<uint8_t>((word >> 24) & 0xFFu);
    if (csLow && _rxIndex < PKT_LEN) {
      _rxPkt[_rxIndex++] = byte;
    }
  }

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
  digitalWrite(PIN_IRQ,
               IRQ_ACTIVE_HIGH ? (asserted ? HIGH : LOW)
                               : (asserted ? LOW : HIGH));
}

void TransportSPI::pushTxByte(uint8_t byte)
{
  pio_sm_put(_pio, _sm, static_cast<uint32_t>(byte) << 24);
}

void TransportSPI::resetRxFrame()
{
  _rxIndex = 0;
  _rxReady = false;
}
