#include <Arduino.h>
#include "EventQueue.h"
#include "TransportProtocol.h"
#include "StatePublisher.h"
#include "ParamBinding.h"
#include "CommandDispatcher.h"

static Param params[] = {
  {"Param0", 0, 0, 0, 127, 1, 1, false},
  {"Param1", 64, 64, 0, 127, 1, 1, false},
};
static EventQueue<16> q;
static StatePublisher pub(q);
static CommandDispatcher disp(params, 2, q);

void setup() {
  Serial.begin(115200);
}

void loop() {
  pub.service();
}
