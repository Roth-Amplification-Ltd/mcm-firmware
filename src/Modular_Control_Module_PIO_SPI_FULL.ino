#include <Arduino.h>

#define BUILD_MODE_CONTROL_SLAVE

#include "EncoderPIO.h"
#include "DebouncedButton.h"
#include "ParamBinding.h"
#include "EventQueue.h"
#include "StatePublisher.h"
#include "TransportSPI.h"

static const int NUM_ENCODERS = 6;
static EventQueue<16> eventQueue;
static StatePublisher publisher(eventQueue);
static TransportSPI spiTransport;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Skeleton main loop; transport integration to be finalized when SPI pins are set.
}
