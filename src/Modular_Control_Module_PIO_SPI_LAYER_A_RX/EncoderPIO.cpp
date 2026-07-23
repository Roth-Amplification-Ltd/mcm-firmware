#include "EncoderPIO.h"
#include "EncoderPIO.pio.h"

static const int8_t QUAD_DELTA[16] = {
  0, +1, -1,  0,
  -1,  0,  0, +1,
  +1,  0,  0, -1,
  0, -1, +1,  0
};

EncoderPIO::EncoderPIO()
: _pio(pio0), _sm(0), _basePin(0), _aBit(0), _bBit(1),
  _count(0), _prevState(0), _started(false) {}

bool EncoderPIO::begin(PIO pio, uint sm, uint gpioA, uint gpioB) {
  _pio = pio; _sm = sm;
  uint8_t base = (gpioA < gpioB) ? gpioA : gpioB;
  uint8_t hi   = (gpioA > gpioB) ? gpioA : gpioB;
  if ((uint)base + 1 != (uint)hi) return false;
  _basePin = base;
  if (gpioA == base) { _aBit = 0; _bBit = 1; } else { _aBit = 1; _bBit = 0; }

  uint offset = pio_add_program(_pio, &encoder_state_stream_program);
  pio_gpio_init(_pio, _basePin);
  pio_gpio_init(_pio, _basePin + 1);
  pio_sm_set_consecutive_pindirs(_pio, _sm, _basePin, 2, false);
  gpio_pull_up(_basePin);
  gpio_pull_up(_basePin + 1);

  pio_sm_config c = encoder_state_stream_program_get_default_config(offset);
  sm_config_set_in_pins(&c, _basePin);
  sm_config_set_clkdiv(&c, 1.0f);
  pio_sm_init(_pio, _sm, offset, &c);
  pio_sm_set_enabled(_pio, _sm, true);

  _count = 0;
  uint8_t raw2 = ((gpio_get(_basePin) & 1u) << 0) | ((gpio_get(_basePin + 1) & 1u) << 1);
  _prevState = canonicalize(raw2);
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) (void)pio_sm_get(_pio, _sm);
  _started = true;
  return true;
}

void EncoderPIO::service() {
  if (!_started) return;
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    uint32_t v = pio_sm_get(_pio, _sm);
    uint8_t raw2 = (uint8_t)(v & 0x03u);
    uint8_t new_state = canonicalize(raw2);
    applyTransition(new_state);
  }
}

int32_t EncoderPIO::getCount() const { return _count; }
void EncoderPIO::setCount(int32_t v) { _count = v; }
int32_t EncoderPIO::getDetents(uint8_t detent_div) const { return detent_div ? _count / (int32_t)detent_div : 0; }

void EncoderPIO::applyTransition(uint8_t new_state) {
  uint8_t idx = (uint8_t)((_prevState << 2) | (new_state & 0x03u));
  _count += (int32_t)QUAD_DELTA[idx];
  _prevState = new_state;
}

uint8_t EncoderPIO::canonicalize(uint8_t raw2) const {
  uint8_t a = (raw2 >> _aBit) & 0x01u;
  uint8_t b = (raw2 >> _bBit) & 0x01u;
  return (uint8_t)((a << 0) | (b << 1));
}
