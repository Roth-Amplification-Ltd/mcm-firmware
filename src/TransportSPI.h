#pragma once
#include <Arduino.h>
#include "hardware/pio.h"

class TransportSPI {
public:
  void begin() {}
  void loadPacket(const uint8_t* pkt8) { memcpy(_pkt, pkt8, 8); _valid = true; }
  void service() {}
private:
  uint8_t _pkt[8];
  bool _valid = false;
};
