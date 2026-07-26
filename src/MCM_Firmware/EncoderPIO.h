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

#include <cstddef>
#include <cstdint>
#include <hardware/pio.h>

namespace Mcm {

class EncoderPIO final {
public:
    EncoderPIO() noexcept = default;

    [[nodiscard]] bool begin(PIO pio,
                             std::uint8_t stateMachine,
                             std::uint8_t gpioA,
                             std::uint8_t gpioB) noexcept;

    void service() noexcept;
    [[nodiscard]] EncoderValue transitionCount() const noexcept {
        return transitionCount_;
    }
    [[nodiscard]] EncoderValue detentCount() const noexcept;
    void setDetentCount(EncoderValue detents) noexcept;

private:
    [[nodiscard]] std::uint8_t canonicalize(std::uint8_t raw) const noexcept;
    void applyTransition(std::uint8_t nextState) noexcept;

    PIO pio_ = pio0;
    std::uint8_t stateMachine_ = 0U;
    std::uint8_t basePin_ = 0U;
    std::uint8_t aBit_ = 0U;
    std::uint8_t bBit_ = 1U;
    EncoderValue transitionCount_ = 0;
    std::uint8_t previousState_ = 0U;
    bool started_ = false;
};

}  // namespace Mcm
