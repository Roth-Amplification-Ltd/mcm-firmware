/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file Modular_Control_Module_PIO_SPI_LAYER_A_RX.ino
 * @brief Historical integrated command, snapshot, RESYNC, and SPI demo.
 *
 * This is the most complete current application-flow reference, but it is not
 * a production sketch:
 *
 * - parameters are placeholders;
 * - EN1–EN6 and their buttons are not scanned;
 * - the directory also contains another `.ino` with setup()/loop();
 * - the nested SPI PIO source is stale relative to the corrected root copy;
 * - TX/event overflow handling is incomplete.
 */
#include <Arduino.h>

#define BUILD_MODE_CONTROL_SLAVE

#include "EventQueue.h"
#include "TransportProtocol.h"
#include "StatePublisher.h"
#include "TransportSPI.h"
#include "ParamBinding.h"
#include "CommandDispatcher.h"
#include "SnapshotFlow.h"

/** Placeholder parameter table; not the six physical controls. */
static Param params[] = {
  { "Param0", 0, 0, 0, 127, 1, 1, false },
  { "Param1", 64, 64, 0, 127, 1, 1, false },
  { "Param2", 32, 32, 0, 127, 1, 1, false }
};
static const uint8_t NUM_PARAMS = sizeof(params) / sizeof(params[0]);

static EventQueue<16> eventQueue;
static SnapshotFlowState snapshotFlow;
static StatePublisher publisher(eventQueue, snapshotFlow);
static TransportSPI spi;
static CommandDispatcher dispatcher(params, NUM_PARAMS, eventQueue, snapshotFlow);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("MCM: CS-framed SPI RX + IRQ data-ready (active HIGH on GPIO11)");
  spi.begin();
}

void loop() {
  // PIO FIFO service and software CS framing must run continuously.
  spi.service();

  // -----------------------------------------------------------------------
  // Master command path
  // -----------------------------------------------------------------------
  if (spi.hasRxPacket()) {
    uint8_t pkt[8];

    if (spi.popRxPacket(pkt)) {
      // RESYNC crosses abstraction boundaries: it clears both transport state
      // and higher-level events before generating a new baseline.
      if (pkt[2] == MSG_CMD_RESYNC) {
        spi.clearTxQueue();
        eventQueue.clear();
        snapshotFlow.snapshot_in_progress = false;
        dispatcher.requestSnapshot();
      } else {
        dispatcher.handlePacket(pkt);
      }
    }
  }

  // -----------------------------------------------------------------------
  // Event publication path
  // -----------------------------------------------------------------------
  publisher.service();

  if (publisher.hasPacket()) {
    uint8_t pkt[8];

    if (publisher.popPacket(pkt)) {
      // Current demo ignores a false return. Production firmware must report
      // overflow and recover with a complete fresh snapshot.
      (void)spi.enqueueTxPacket(pkt);
    }
  }

  // SLAVE_IRQ is managed by TransportSPI from software TX queue state.
}
