/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "McmTypes.h"

#include <cstddef>
#include <cstdint>

namespace Mcm::Hardware {

inline constexpr std::uint8_t SpiIrqPin = 11U;
inline constexpr std::uint8_t SpiSckPin = 12U;
inline constexpr std::uint8_t SpiCsPin = 13U;
inline constexpr std::uint8_t SpiMisoPin = 14U;
inline constexpr std::uint8_t SpiMosiPin = 15U;
inline constexpr bool SpiIrqActiveHigh = true;

inline constexpr std::uint8_t SpiPioIndex = 1U;
inline constexpr std::uint8_t SpiStateMachine = 2U;

inline constexpr std::size_t SpiTxQueueDepth = 16U;
inline constexpr std::size_t PublicationQueueDepth = 24U;
inline constexpr std::size_t SnapshotPacketCount = ControlCount + 3U;

inline constexpr std::uint8_t EncoderTransitionsPerDetent = 4U;
inline constexpr std::size_t MaxEncoderSamplesPerService = 8U;
inline constexpr Milliseconds ButtonDebounceMs = 15U;
inline constexpr Milliseconds ButtonLongPressMs = 600U;
inline constexpr std::uint32_t SerialBaud = 115200U;

static_assert(PacketLength == 8U, "SPI protocol packet length is fixed.");
static_assert(SpiTxQueueDepth >= SnapshotPacketCount,
              "The TX queue must hold at least one complete snapshot.");
static_assert(PublicationQueueDepth >= (ControlCount + 1U),
              "The publication queue must hold one complete change scan.");
static_assert(SpiPioIndex < 2U, "RP2040 provides PIO0 and PIO1 only.");
static_assert(SpiStateMachine < 4U,
              "Each RP2040 PIO block provides four state machines.");

}  // namespace Mcm::Hardware
