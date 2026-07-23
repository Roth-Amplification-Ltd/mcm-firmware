#include <Arduino.h>
#include "EventQueue.h"
#include "TransportProtocol.h"
#include "StatePublisher.h"
#include "TransportSPI.h"
#include "ParamBinding.h"
#include "CommandDispatcher.h"

/*
 * The MCM is an SPI-only peripheral. The pin map is centralized in
 * HardwareConfig.h and matches the current KiCad project.
 */
static Param params[] = {
  {"Param0", 0, 0, 0, 127, 1, 1, false},
  {"Param1", 64, 64, 0, 127, 1, 1, false},
};

static constexpr uint8_t PARAM_COUNT = sizeof(params) / sizeof(params[0]);
static EventQueue<16> eventQueue;
static StatePublisher publisher(eventQueue);
static CommandDispatcher dispatcher(params, PARAM_COUNT, eventQueue);
static TransportSPI spi;

void setup() {
  Serial.begin(115200);
  spi.begin();
}

void loop() {
  // Service the PIO transport on every pass so RX and TX FIFOs cannot starve.
  spi.service();

  // Master -> MCM command path over MOSI.
  if (spi.hasRxPacket()) {
    uint8_t packet[McmHardware::SPI_PACKET_LENGTH];
    if (spi.popRxPacket(packet)) {
      dispatcher.handlePacket(packet);
    }
  }

  // Local state/event -> master response path over MISO.
  publisher.service();
  if (publisher.hasPacket()) {
    uint8_t packet[McmHardware::SPI_PACKET_LENGTH];
    if (publisher.popPacket(packet)) {
      // A full transport queue currently drops the newest packet. Future
      // application code may expose a diagnostic counter if required.
      (void)spi.enqueueTxPacket(packet);
    }
  }
}
