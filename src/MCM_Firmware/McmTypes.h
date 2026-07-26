/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace Mcm {

using Byte = std::uint8_t;
using ControlIndex = std::uint8_t;
using Milliseconds = std::uint32_t;
using EncoderValue = std::int32_t;

inline constexpr std::size_t ControlCount = 6U;
inline constexpr std::size_t PacketLength = 8U;
inline constexpr std::uint8_t ValidControlMask = 0x3FU;

static_assert(std::numeric_limits<Byte>::digits == 8,
              "The protocol requires eight-bit bytes.");
static_assert(sizeof(EncoderValue) >= 4U,
              "EncoderValue must represent a signed 24-bit payload.");
static_assert(ControlCount <= 8U,
              "Button state is represented by one bitmap byte.");

[[nodiscard]] constexpr bool isValidControlIndex(ControlIndex index) noexcept {
    return static_cast<std::size_t>(index) < ControlCount;
}

}  // namespace Mcm
