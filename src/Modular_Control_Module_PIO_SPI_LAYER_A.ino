#include <Arduino.h>

#define BUILD_MODE_CONTROL_SLAVE

#include "EventQueue.h"
#include "TransportProtocol.h"
#include "StatePublisher.h"
#include "TransportSPI.h"
#include "ParamBinding.h"
#include "CommandDispatcher.h"

// -----------------------------------------------------------------------------
// Example parameter table (placeholder)
// -----------------------------------------------------------------------------
static Param params[] = {
  { "Param0", 0, 0, 0, 127, 1, 1, false },
  { "Param1", 64, 64, 0, 127, 1, 1, false },
  { "Param2", 32, 32, 0, 127, 1, 1, false }
};

static const uint8_t NUM_PARAMS = sizeof(params) / sizeof(params[0]);

static EventQueue<16> eventQueue;
static StatePublisher publisher(eventQueue);
static TransportSPI   spi;
static CommandDispatcher dispatcher(params, NUM_PARAMS, eventQueue);

void setup() {
  Serial.begin(115200);
  spi.begin();
}

void loop() {
  // --- RX path (placeholder hook) ---
  // In the next iteration, TransportSPI will expose RX packet retrieval.
  // For now, commands are injected via SPI test harness.

  // --- TX path ---
  publisher.service();
  if (publisher.hasPacket()) {
    uint8_t pkt[8];
    if (publisher.popPacket(pkt)) {
      spi.loadPacket(pkt);
    }
  }
  spi.service();
}
