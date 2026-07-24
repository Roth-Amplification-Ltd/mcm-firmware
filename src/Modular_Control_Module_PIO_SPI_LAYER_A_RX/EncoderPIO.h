#pragma once
#include <Arduino.h>
#include "hardware/pio.h"

/**
 * @class EncoderPIO
 * @brief Quadrature transition accumulator fed by an RP2040 PIO state stream.
 *
 * The PIO program reports two-bit pin states only when they change. This class
 * canonicalizes caller-defined A/B phase order, applies a 16-entry transition
 * table, and accumulates raw quadrature transitions.
 *
 * Requirements:
 * - A and B GPIO numbers must be consecutive;
 * - caller must allocate a valid, unused state machine;
 * - caller must service the RX FIFO frequently enough to avoid overflow;
 * - six encoders require state machines across both PIO blocks.
 */
class EncoderPIO {
public:
  EncoderPIO();

  /**
   * Load/configure the encoder PIO program and begin sampling.
   * @return false when gpioA and gpioB are not consecutive.
   */
  bool begin(PIO pio, uint sm, uint gpioA, uint gpioB);

  /** Drain every pending two-bit state sample and update the raw count. */
  void service();

  int32_t getCount() const;
  void setCount(int32_t v);

  /** Convert raw transitions to detents by integer division. */
  int32_t getDetents(uint8_t detent_div = 4) const;

private:
  void applyTransition(uint8_t new_state);
  uint8_t canonicalize(uint8_t raw2) const;

  PIO _pio;
  uint _sm;
  uint8_t _basePin;
  uint8_t _aBit;
  uint8_t _bBit;
  volatile int32_t _count;
  uint8_t _prevState;
  bool _started;
};
