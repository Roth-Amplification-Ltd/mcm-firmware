#include "TransportSPI.h"
#include "SpiSlaveM0.pio.h"

TransportSPI::TransportSPI()
: _pio(pio1), _sm(2), _offset(0),
  _txValid(false), _txIndex(0),
  _rxIndex(0), _rxReady(false)
{}

void TransportSPI::begin() {
  pinMode(PIN_SCK, INPUT);
  pinMode(PIN_CS, INPUT_PULLUP);
  pinMode(PIN_MOSI, INPUT);

  _offset = pio_add_program(_pio, &spi_slave_m0_msb_rx_program);

  pio_gpio_init(_pio, PIN_MISO);
  pio_sm_set_consecutive_pindirs(_pio, _sm, PIN_MISO, 1, true);

  pio_sm_config c = spi_slave_m0_msb_rx_program_get_default_config(_offset);
  sm_config_set_in_pins(&c, PIN_MOSI);
  sm_config_set_out_pins(&c, PIN_MISO, 1);
  sm_config_set_out_shift(&c, false, false, 32);
  sm_config_set_in_shift(&c, false, false, 32);
  sm_config_set_clkdiv(&c, 1.0f);

  pio_sm_init(_pio, _sm, _offset, &c);
  pio_sm_set_enabled(_pio, _sm, true);
}

void TransportSPI::loadTxPacket(const uint8_t* pkt8) {
  memcpy(_txPkt, pkt8, PKT_LEN);
  _txIndex = 0;
  _txValid = true;
}

bool TransportSPI::hasRxPacket() const {
  return _rxReady;
}

bool TransportSPI::popRxPacket(uint8_t* pkt8) {
  if (!_rxReady) return false;
  memcpy(pkt8, _rxPkt, PKT_LEN);
  _rxReady = false;
  return true;
}

void TransportSPI::service() {
  // TX path
  if (_txValid) {
    while (_txIndex < PKT_LEN && !pio_sm_is_tx_fifo_full(_pio, _sm)) {
      pushTxByte(_txPkt[_txIndex++]);
    }
    if (_txIndex >= PKT_LEN) _txValid = false;
  }

  // RX path
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    uint32_t v = pio_sm_get(_pio, _sm);
    _rxPkt[_rxIndex++] = (uint8_t)(v & 0xFF);
    if (_rxIndex >= PKT_LEN) {
      _rxIndex = 0;
      _rxReady = true;
    }
  }
}

void TransportSPI::pushTxByte(uint8_t b) {
  pio_sm_put(_pio, _sm, ((uint32_t)b) << 24);
}
