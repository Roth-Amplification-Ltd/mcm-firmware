/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "EncoderPIO.h"

#include "EncoderPIO.pio.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#include <hardware/gpio.h>

namespace Mcm {
namespace {

inline constexpr std::array<std::int8_t, 16U> QuadratureDelta{{
    0, 1, -1, 0,
    -1, 0, 0, 1,
    1, 0, 0, -1,
    0, -1, 1, 0,
}};

struct ProgramLocation final {
    bool loaded = false;
    std::uint8_t offset = 0U;
};

std::array<ProgramLocation, 2U> programLocations{};

[[nodiscard]] std::size_t pioIndex(PIO pio) noexcept {
    return (pio == pio0) ? 0U : 1U;
}

[[nodiscard]] std::uint8_t programOffset(PIO pio) noexcept {
    ProgramLocation& location = programLocations[pioIndex(pio)];
    if (!location.loaded) {
        location.offset = static_cast<std::uint8_t>(
            pio_add_program(pio, &encoder_state_stream_program));
        location.loaded = true;
    }
    return location.offset;
}

}  // namespace

bool EncoderPIO::begin(PIO pio,
                       std::uint8_t stateMachine,
                       std::uint8_t gpioA,
                       std::uint8_t gpioB) noexcept {
    const std::uint8_t low = (gpioA < gpioB) ? gpioA : gpioB;
    const std::uint8_t high = (gpioA > gpioB) ? gpioA : gpioB;

    if (static_cast<std::uint8_t>(low + 1U) != high || stateMachine >= 4U) {
        return false;
    }

    pio_ = pio;
    stateMachine_ = stateMachine;
    basePin_ = low;
    aBit_ = (gpioA == low) ? 0U : 1U;
    bBit_ = (gpioB == low) ? 0U : 1U;

    const std::uint8_t offset = programOffset(pio_);
    pio_gpio_init(pio_, basePin_);
    pio_gpio_init(pio_, static_cast<std::uint8_t>(basePin_ + 1U));
    pio_sm_set_consecutive_pindirs(pio_, stateMachine_, basePin_, 2U, false);
    gpio_pull_up(basePin_);
    gpio_pull_up(static_cast<std::uint8_t>(basePin_ + 1U));

    pio_sm_config config =
        encoder_state_stream_program_get_default_config(offset);
    sm_config_set_in_pins(&config, basePin_);
    sm_config_set_in_shift(&config, false, false, 32U);
    sm_config_set_clkdiv(&config, 1.0F);

    pio_sm_init(pio_, stateMachine_, offset, &config);
    pio_sm_clear_fifos(pio_, stateMachine_);
    pio_sm_set_enabled(pio_, stateMachine_, true);

    const std::uint8_t raw = static_cast<std::uint8_t>(
        (gpio_get(basePin_) & 0x01U) |
        ((gpio_get(static_cast<std::uint8_t>(basePin_ + 1U)) & 0x01U) << 1U));

    transitionCount_ = 0;
    previousState_ = canonicalize(raw);
    started_ = true;
    return true;
}

void EncoderPIO::service() noexcept {
    if (!started_) {
        return;
    }

    for (std::size_t sample = 0U;
         sample < Hardware::MaxEncoderSamplesPerService;
         ++sample) {
        if (pio_sm_is_rx_fifo_empty(pio_, stateMachine_)) {
            break;
        }

        const std::uint32_t word = pio_sm_get(pio_, stateMachine_);
        applyTransition(canonicalize(static_cast<std::uint8_t>(word & 0x03U)));
    }
}

EncoderValue EncoderPIO::detentCount() const noexcept {
    return transitionCount_ /
           static_cast<EncoderValue>(Hardware::EncoderTransitionsPerDetent);
}

void EncoderPIO::setDetentCount(EncoderValue detents) noexcept {
    const EncoderValue multiplier =
        static_cast<EncoderValue>(Hardware::EncoderTransitionsPerDetent);
    const EncoderValue maximum =
        std::numeric_limits<EncoderValue>::max() / multiplier;
    const EncoderValue minimum =
        std::numeric_limits<EncoderValue>::min() / multiplier;
    const EncoderValue bounded =
        (detents > maximum) ? maximum : ((detents < minimum) ? minimum : detents);
    transitionCount_ = bounded * multiplier;
}

void EncoderPIO::applyTransition(std::uint8_t nextState) noexcept {
    const std::uint8_t lookupIndex = static_cast<std::uint8_t>(
        static_cast<std::uint8_t>(previousState_ << 2U) |
        static_cast<std::uint8_t>(nextState & 0x03U));
    const std::int8_t delta = QuadratureDelta[lookupIndex];

    if (delta > 0) {
        if (transitionCount_ < std::numeric_limits<EncoderValue>::max()) {
            ++transitionCount_;
        }
    } else if (delta < 0) {
        if (transitionCount_ > std::numeric_limits<EncoderValue>::min()) {
            --transitionCount_;
        }
    }

    previousState_ = nextState;
}

std::uint8_t EncoderPIO::canonicalize(std::uint8_t raw) const noexcept {
    const std::uint8_t a = static_cast<std::uint8_t>((raw >> aBit_) & 0x01U);
    const std::uint8_t b = static_cast<std::uint8_t>((raw >> bBit_) & 0x01U);
    return static_cast<std::uint8_t>(a | static_cast<std::uint8_t>(b << 1U));
}

}  // namespace Mcm
