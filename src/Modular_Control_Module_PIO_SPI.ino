/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file Modular_Control_Module_PIO_SPI.ino
 * @brief Newer hardware-aligned SPI integration sketch.
 *
 * This sketch demonstrates the intended application-layer data flow:
 *
 *   SPI MOSI -> TransportSPI -> CommandDispatcher -> EventQueue
 *   EventQueue -> StatePublisher -> TransportSPI -> SPI MISO
 *
 * The corrected root TransportSPI and SpiSlaveM0.pio files match the current
 * KiCad host-interface pins.
 *
 * @warning CURRENT REPOSITORY STATUS
 * The support headers included below were moved into the nested historical
 * source directory by commit 0734fb17. As a result, this sketch is not a
 * self-contained build target on main until the source trees are consolidated.
 * See docs/CURRENT_STATUS.md and docs/IMPLEMENTATION_ROADMAP.md.
 *
 * @warning CONTROL STATUS
 * The parameter array remains a two-entry placeholder. The six physical
 * encoders and six push-buttons are not instantiated or scanned here.
 */
#include <Arduino.h>
#include "EventQueue.h"
#include "TransportProtocol.h"
#include "StatePublisher.h"
#include "TransportSPI.h"
#include "ParamBinding.h"
#include "CommandDispatcher.h"

/**
 * Placeholder authoritative parameter table.
 *
 * Production firmware should define indexes 0..5 as EN1..EN6 and bind each
 * entry to one physical encoder. Button state requires its own normalized
 * model and packet representation.
 */
static Param params[] = {
  {"Param0", 0, 0, 0, 127, 1, 1, false},
  {"Param1", 64, 64, 0, 127, 1, 1, false},
};

static constexpr uint8_t PARAM_COUNT = sizeof(params) / sizeof(params[0]);

// Internal pipeline objects. These are intentionally static so they exist for
// the entire firmware lifetime and require no dynamic allocation.
static EventQueue<16> eventQueue;
static StatePublisher publisher(eventQueue);
static CommandDispatcher dispatcher(params, PARAM_COUNT, eventQueue);
static TransportSPI spi;

void setup() {
  // Serial is currently diagnostic only. The external control protocol uses
  // SPI, not USB serial and not I2C.
  Serial.begin(115200);

  // Configure host-interface pins and enable the PIO SPI state machine.
  spi.begin();
}

void loop() {
  // Transport service must run frequently. It drains bytes received by PIO,
  // recognizes CS frame boundaries, and supplies outbound bytes to PIO.
  spi.service();

  // -----------------------------------------------------------------------
  // Master -> MCM command path
  // -----------------------------------------------------------------------
  if (spi.hasRxPacket()) {
    uint8_t packet[McmHardware::SPI_PACKET_LENGTH];

    if (spi.popRxPacket(packet)) {
      // The dispatcher, not the transport, validates sync/version/CRC and owns
      // command semantics.
      dispatcher.handlePacket(packet);
    }
  }

  // -----------------------------------------------------------------------
  // Local event -> master response path
  // -----------------------------------------------------------------------
  publisher.service();

  if (publisher.hasPacket()) {
    uint8_t packet[McmHardware::SPI_PACKET_LENGTH];

    if (publisher.popPacket(packet)) {
      // A false return means the eight-packet TX ring is full. Current code
      // discards that information. Production firmware must count/report the
      // overflow and normally force a later coherent RESYNC.
      (void)spi.enqueueTxPacket(packet);
    }
  }
}
