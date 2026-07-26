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

#include <cstdint>


namespace Mcm {

struct ButtonUpdate final {
    bool pressedEdge = false;
    bool releasedEdge = false;
    bool longHoldEdge = false;
};

class DebouncedButton final {
public:
    enum class State : std::uint8_t {
        Released = 0U,
        DebouncingPress,
        Pressed,
        LongHeld,
        DebouncingReleasePressed,
        DebouncingReleaseLong,
    };

    DebouncedButton() noexcept = default;

    void begin(std::uint8_t pin, Milliseconds now) noexcept;
    [[nodiscard]] ButtonUpdate service(Milliseconds now) noexcept;
    [[nodiscard]] ButtonUpdate serviceRaw(bool rawPressed,
                                          Milliseconds now) noexcept;

    [[nodiscard]] bool isPressed() const noexcept;
    [[nodiscard]] bool isLongHeld() const noexcept;
    [[nodiscard]] State state() const noexcept { return state_; }

private:
    [[nodiscard]] static Milliseconds elapsed(Milliseconds now,
                                              Milliseconds then) noexcept {
        return static_cast<Milliseconds>(now - then);
    }

    std::uint8_t pin_ = 0U;
    State state_ = State::Released;
    Milliseconds transitionStarted_ = 0U;
    Milliseconds pressStarted_ = 0U;
};

}  // namespace Mcm
