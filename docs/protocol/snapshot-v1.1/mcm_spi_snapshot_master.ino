/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <Arduino.h>
#include <SPI.h>
#include "mcm_spi_snapshot_protocol.h"

/*
 * Generic Arduino-style SPI-master example.
 *
 * Adjust these pins and SPI instance for the selected master MCU. The MCM
 * itself remains the SPI peripheral/slave.
 */
static constexpr uint8_t PIN_MCM_CS  = 10;
static constexpr uint8_t PIN_MCM_IRQ = 9;
static constexpr uint32_t SPI_HZ = 1000000;
static constexpr uint32_t IRQ_TIMEOUT_MS = 100;
static constexpr uint8_t MAX_SNAPSHOT_PACKETS = 9;

struct SnapshotAssembler {
  mcm_control_snapshot_t candidate{};
  uint8_t seenEncoderMask = 0;
  bool seenButtons = false;
  bool active = false;

  void reset() {
    memset(&candidate, 0, sizeof(candidate));
    seenEncoderMask = 0;
    seenButtons = false;
    active = false;
  }

  bool consume(const mcm_spi_packet_t &packet) {
    if (!mcm_spi_packet_is_valid(&packet)) {
      reset();
      return false;
    }

    switch (packet.byte[2]) {
      case MCM_MSG_SNAPSHOT_BEGIN:
        reset();
        if (packet.byte[3] != MCM_CONTROL_COUNT) {
          return false;
        }
        active = true;
        candidate.sequence_id = mcm_spi_snapshot_sequence(&packet);
        candidate.status_flags = packet.byte[6];
        return false;

      case MCM_MSG_PARAM_STATE: {
        if (!active || packet.byte[3] >= MCM_CONTROL_COUNT) {
          reset();
          return false;
        }
        const uint8_t index = packet.byte[3];
        const uint8_t mask = (uint8_t)(1u << index);
        if ((seenEncoderMask & mask) != 0u) {
          reset();  // Duplicate index makes the snapshot ambiguous.
          return false;
        }
        candidate.encoder[index] = mcm_spi_param_state_value(&packet);
        seenEncoderMask |= mask;
        return false;
      }

      case MCM_MSG_BUTTON_STATE:
        if (!active || packet.byte[3] != MCM_BUTTON_BANK_0 ||
            packet.byte[6] != MCM_CONTROL_COUNT || seenButtons) {
          reset();
          return false;
        }
        candidate.button_pressed_bitmap = (uint8_t)(packet.byte[4] & 0x3Fu);
        candidate.button_long_bitmap = (uint8_t)(packet.byte[5] & 0x3Fu);
        seenButtons = true;
        return false;

      case MCM_MSG_SNAPSHOT_END: {
        const bool complete = active &&
          packet.byte[3] == MCM_CONTROL_COUNT &&
          mcm_spi_snapshot_sequence(&packet) == candidate.sequence_id &&
          packet.byte[6] == candidate.status_flags &&
          seenEncoderMask == 0x3Fu &&
          seenButtons;
        active = false;
        return complete;
      }

      default:
        return false;
    }
  }
};

static SnapshotAssembler assembler;
static mcm_control_snapshot_t latestSnapshot{};

static void transferPacket(const mcm_spi_packet_t &tx, mcm_spi_packet_t &rx) {
  SPI.beginTransaction(SPISettings(SPI_HZ, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_MCM_CS, LOW);
  for (uint8_t i = 0; i < MCM_SPI_PACKET_SIZE; ++i) {
    rx.byte[i] = SPI.transfer(tx.byte[i]);
  }
  digitalWrite(PIN_MCM_CS, HIGH);
  SPI.endTransaction();
}

static bool requestSnapshot(mcm_control_snapshot_t &output) {
  mcm_spi_packet_t command{};
  mcm_spi_packet_t ignoredRx{};
  mcm_spi_make_get_snapshot(&command);
  transferPacket(command, ignoredRx);

  const uint32_t started = millis();
  while (digitalRead(PIN_MCM_IRQ) == LOW) {
    if ((millis() - started) >= IRQ_TIMEOUT_MS) {
      return false;
    }
  }

  assembler.reset();
  mcm_spi_packet_t dummy{};  // Eight zero bytes on MOSI while reading MISO.

  for (uint8_t count = 0; count < MAX_SNAPSHOT_PACKETS; ++count) {
    mcm_spi_packet_t response{};
    transferPacket(dummy, response);

    if (assembler.consume(response)) {
      output = assembler.candidate;
      return true;
    }
  }
  return false;
}

static void printSnapshot(const mcm_control_snapshot_t &snapshot) {
  Serial.print("snapshot #");
  Serial.print(snapshot.sequence_id);
  Serial.print(" status=0x");
  Serial.println(snapshot.status_flags, HEX);

  for (uint8_t i = 0; i < MCM_CONTROL_COUNT; ++i) {
    Serial.print("EN");
    Serial.print(i + 1);
    Serial.print(" value=");
    Serial.print(snapshot.encoder[i]);
    Serial.print(" button=");
    Serial.print(mcm_spi_button_is_pressed(snapshot.button_pressed_bitmap, i)
                   ? "pressed" : "released");
    Serial.print(" long=");
    Serial.println(mcm_spi_button_is_pressed(snapshot.button_long_bitmap, i)
                     ? "yes" : "no");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MCM_CS, OUTPUT);
  digitalWrite(PIN_MCM_CS, HIGH);
  pinMode(PIN_MCM_IRQ, INPUT);
  SPI.begin();
}

void loop() {
  if (requestSnapshot(latestSnapshot)) {
    printSnapshot(latestSnapshot);
  } else {
    Serial.println("MCM snapshot failed validation or timed out");
  }
  delay(20);  // Example 50 Hz snapshot rate.
}
