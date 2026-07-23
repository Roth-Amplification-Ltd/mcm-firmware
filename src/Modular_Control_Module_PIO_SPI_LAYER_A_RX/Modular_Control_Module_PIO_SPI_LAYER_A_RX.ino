#include <Arduino.h>

#define BUILD_MODE_CONTROL_SLAVE

#include "EventQueue.h"
#include "TransportProtocol.h"
#include "StatePublisher.h"
#include "TransportSPI.h"
#include "ParamBinding.h"
#include "CommandDispatcher.h"
#include "SnapshotFlow.h"

/*****************************************************************************************
 *  DEMO PARAMS (placeholder)
 *  ------------------------
 *  Your real firmware likely has many more params mapped to encoders.
 *  This sketch demonstrates:
 *    - Master->Slave commands over SPI MOSI (CS-framed)
 *    - Slave->Master state packets over SPI MISO
 *    - IRQ pin asserted HIGH when TX packets are pending
 *
 *  IRQ PIN:
 *    GPIO11 (Pin 14) active HIGH
 *****************************************************************************************/

static Param params[] = {
  { "Param0", 0, 0, 0, 127, 1, 1, false },
  { "Param1", 64, 64, 0, 127, 1, 1, false },
  { "Param2", 32, 32, 0, 127, 1, 1, false }
};
static const uint8_t NUM_PARAMS = sizeof(params) / sizeof(params[0]);

static EventQueue<16> eventQueue;
static SnapshotFlowState snapshotFlow;
static StatePublisher publisher(eventQueue, snapshotFlow);
static TransportSPI   spi;
static CommandDispatcher dispatcher(params, NUM_PARAMS, eventQueue, snapshotFlow);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("MCM: CS-framed SPI RX + IRQ data-ready (active HIGH on GPIO11)");
  spi.begin();
}

void loop() {
  // Always service the transport frequently (drains RX FIFO + feeds TX FIFO)
  spi.service();

  // ----------------- RX: MOSI -> Command Dispatcher -----------------
  if (spi.hasRxPacket()) {
    uint8_t pkt[8];
    if (spi.popRxPacket(pkt)) {
            // Layer D: RESYNC command is special because it must clear BOTH:
      //   1) the internal event queue (drop stale events)
      //   2) the transport TX queue (drop stale outbound packets)
      //
      // Then it immediately queues a fresh framed snapshot so the master can
      // re-establish an authoritative baseline.
      if (pkt[2] == MSG_CMD_RESYNC) {
        // Drop any pending outbound data and deassert IRQ immediately.
        spi.clearTxQueue();

        // Drop pending internal events (including any partial snapshot).
        eventQueue.clear();

        // Reset snapshot flow state so a new snapshot can be queued cleanly.
        snapshotFlow.snapshot_in_progress = false;

        // Queue a fresh framed snapshot (BEGIN, PARAM_STATE×N, END).
        dispatcher.requestSnapshot();
      } else {
        dispatcher.handlePacket(pkt);
      }
    }
  }

  // ----------------- Publish pipeline: Events -> Packets -> TX Queue -----------------
  publisher.service();
  if (publisher.hasPacket()) {
    uint8_t pkt[8];
    if (publisher.popPacket(pkt)) {
      // Enqueue for TX; if queue is full, packet is dropped (rare; can tune depth)
      spi.enqueueTxPacket(pkt);
    }
  }

  // NOTE: IRQ pin is managed automatically inside TransportSPI based on TX queue.
}
