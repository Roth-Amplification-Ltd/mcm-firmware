/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "HardwareConfig.h"
#include "McmTypes.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace Mcm::Hardware {

struct ControlHardware final {
    std::uint8_t encoderAPin;
    std::uint8_t encoderBPin;
    std::uint8_t buttonPin;
    std::uint8_t pioIndex;
    std::uint8_t stateMachine;
};

inline constexpr std::array<ControlHardware, ControlCount> Controls{{
    {4U, 5U, 3U, 0U, 0U},    // EN1 / packet index 0
    {7U, 6U, 8U, 0U, 1U},    // EN2 / packet index 1
    {19U, 20U, 18U, 0U, 2U}, // EN3 / packet index 2
    {1U, 2U, 0U, 0U, 3U},    // EN4 / packet index 3
    {26U, 27U, 28U, 1U, 0U}, // EN5 / packet index 4
    {25U, 24U, 23U, 1U, 1U}, // EN6 / packet index 5
}};

[[nodiscard]] constexpr bool pinsAreAdjacent(const ControlHardware& control) noexcept {
    const std::uint8_t low =
        (control.encoderAPin < control.encoderBPin) ? control.encoderAPin
                                                   : control.encoderBPin;
    const std::uint8_t high =
        (control.encoderAPin > control.encoderBPin) ? control.encoderAPin
                                                   : control.encoderBPin;
    return static_cast<std::uint8_t>(low + 1U) == high;
}

[[nodiscard]] constexpr bool allEncoderPinsAreAdjacent() noexcept {
    for (const ControlHardware& control : Controls) {
        if (!pinsAreAdjacent(control)) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] constexpr bool encoderResourcesAvoidSpiStateMachine() noexcept {
    for (const ControlHardware& control : Controls) {
        if ((control.pioIndex == SpiPioIndex) &&
            (control.stateMachine == SpiStateMachine)) {
            return false;
        }
    }
    return true;
}

static_assert(Controls.size() == ControlCount,
              "The hardware map must describe all six controls.");
static_assert(allEncoderPinsAreAdjacent(),
              "The encoder PIO implementation requires adjacent A/B GPIOs.");
static_assert(encoderResourcesAvoidSpiStateMachine(),
              "An encoder cannot share the SPI PIO state machine.");
static_assert(Controls[0U].buttonPin == 3U && Controls[5U].buttonPin == 23U,
              "Control ordering must remain EN1 through EN6.");

}  // namespace Mcm::Hardware
