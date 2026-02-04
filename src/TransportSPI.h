
#pragma once
#include <Arduino.h>

class TransportSPI {
public:
  void begin() {}
  void loadPacket(const uint8_t* pkt) { memcpy(_buf, pkt, 8); _valid = true; }
  void service() {}
private:
  uint8_t _buf[8];
  bool _valid = false;
};
