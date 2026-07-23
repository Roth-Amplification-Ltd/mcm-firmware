#pragma once
#include <Arduino.h>
#include "hardware/pio.h"

class EncoderPIO {
public:
  EncoderPIO();
  bool begin(PIO pio, uint sm, uint gpioA, uint gpioB);
  void service();
  int32_t getCount() const;
  void setCount(int32_t v);
  int32_t getDetents(uint8_t detent_div = 4) const;
private:
  void applyTransition(uint8_t new_state);
  uint8_t canonicalize(uint8_t raw2) const;

  PIO _pio; uint _sm;
  uint8_t _basePin, _aBit, _bBit;
  volatile int32_t _count;
  uint8_t _prevState;
  bool _started;
};
