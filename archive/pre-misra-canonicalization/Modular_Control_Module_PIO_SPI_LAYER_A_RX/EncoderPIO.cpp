/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "EncoderPIO.h"
#include "EncoderPIO.pio.h"

/**
 * Quadrature transition lookup.
 *
 * Index = previous two-bit state in bits 3..2 plus new state in bits 1..0.
 * Legal one-bit Gray-code transitions produce +1 or -1. Same-state samples and
 * invalid two-bit jumps produce zero.
 */
static const int8_t QUAD_DELTA[16] = {
   0, +1, -1,  0,
  -1,  0,  0, +1,
  +1,  0,  0, -1,
   0, -1, +1,  0
};

EncoderPIO::EncoderPIO()
: _pio(pio0),
  _sm(0),
  _basePin(0),
  _aBit(0),
  _bBit(1),
  _count(0),
  _prevState(0),
  _started(false)
{}

bool EncoderPIO::begin(PIO pio, uint sm, uint gpioA, uint gpioB) {
  _pio = pio;
  _sm = sm;

  // PIO IN reads a consecutive pin group from one base. Reject non-adjacent
  // phase pins rather than silently sampling an unrelated GPIO.
  const uint8_t base = (gpioA < gpioB) ? gpioA : gpioB;
  const uint8_t high = (gpioA > gpioB) ? gpioA : gpioB;
  if ((uint)base + 1 != (uint)high) return false;

  _basePin = base;

  // Preserve semantic A/B order even when phase A is the higher-numbered pin,
  // as occurs on EN2 and EN6 in the current board design.
  if (gpioA == base) {
    _aBit = 0;
    _bBit = 1;
  } else {
    _aBit = 1;
    _bBit = 0;
  }

  // Current implementation loads another program copy for each instance. A
  // production allocator should load once per PIO block and share the offset.
  const uint offset = pio_add_program(_pio, &encoder_state_stream_program);

  pio_gpio_init(_pio, _basePin);
  pio_gpio_init(_pio, _basePin + 1);
  pio_sm_set_consecutive_pindirs(_pio, _sm, _basePin, 2, false);
  gpio_pull_up(_basePin);
  gpio_pull_up(_basePin + 1);

  pio_sm_config config = encoder_state_stream_program_get_default_config(offset);
  sm_config_set_in_pins(&config, _basePin);
  sm_config_set_clkdiv(&config, 1.0f);

  pio_sm_init(_pio, _sm, offset, &config);
  pio_sm_set_enabled(_pio, _sm, true);

  _count = 0;

  // Seed previous state from the physical pins so the first reported change is
  // interpreted relative to reality rather than an assumed 00 state.
  const uint8_t raw2 =
      ((gpio_get(_basePin) & 1u) << 0)
    | ((gpio_get(_basePin + 1) & 1u) << 1);
  _prevState = canonicalize(raw2);

  // Discard any startup samples accumulated before initialization completed.
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    (void)pio_sm_get(_pio, _sm);
  }

  _started = true;
  return true;
}

void EncoderPIO::service() {
  if (!_started) return;

  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    const uint32_t word = pio_sm_get(_pio, _sm);
    const uint8_t raw2 = (uint8_t)(word & 0x03u);
    const uint8_t newState = canonicalize(raw2);
    applyTransition(newState);
  }
}

int32_t EncoderPIO::getCount() const { return _count; }
void EncoderPIO::setCount(int32_t v) { _count = v; }

int32_t EncoderPIO::getDetents(uint8_t detent_div) const {
  return detent_div ? _count / (int32_t)detent_div : 0;
}

void EncoderPIO::applyTransition(uint8_t new_state) {
  const uint8_t tableIndex =
      (uint8_t)((_prevState << 2) | (new_state & 0x03u));

  _count += (int32_t)QUAD_DELTA[tableIndex];
  _prevState = new_state;
}

uint8_t EncoderPIO::canonicalize(uint8_t raw2) const {
  const uint8_t a = (raw2 >> _aBit) & 0x01u;
  const uint8_t b = (raw2 >> _bBit) & 0x01u;
  return (uint8_t)((a << 0) | (b << 1));
}
