/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "DebouncedButton.h"

#include <Arduino.h>

namespace Mcm {

void DebouncedButton::begin(std::uint8_t pin, Milliseconds now) noexcept {
    pin_ = pin;
    pinMode(pin_, INPUT_PULLUP);

    const bool pressed = (digitalRead(pin_) == LOW);
    state_ = pressed ? State::Pressed : State::Released;
    transitionStarted_ = now;
    pressStarted_ = now;
}

ButtonUpdate DebouncedButton::service(Milliseconds now) noexcept {
    return serviceRaw(digitalRead(pin_) == LOW, now);
}

ButtonUpdate DebouncedButton::serviceRaw(bool rawPressed,
                                         Milliseconds now) noexcept {
    ButtonUpdate update{};

    switch (state_) {
        case State::Released:
            if (rawPressed) {
                state_ = State::DebouncingPress;
                transitionStarted_ = now;
            }
            break;

        case State::DebouncingPress:
            if (!rawPressed) {
                state_ = State::Released;
            } else if (elapsed(now, transitionStarted_) >=
                       Hardware::ButtonDebounceMs) {
                state_ = State::Pressed;
                pressStarted_ = now;
                update.pressedEdge = true;
            }
            break;

        case State::Pressed:
            if (!rawPressed) {
                state_ = State::DebouncingReleasePressed;
                transitionStarted_ = now;
            } else if (elapsed(now, pressStarted_) >=
                       Hardware::ButtonLongPressMs) {
                state_ = State::LongHeld;
                update.longHoldEdge = true;
            }
            break;

        case State::LongHeld:
            if (!rawPressed) {
                state_ = State::DebouncingReleaseLong;
                transitionStarted_ = now;
            }
            break;

        case State::DebouncingReleasePressed:
            if (rawPressed) {
                state_ = State::Pressed;
            } else if (elapsed(now, transitionStarted_) >=
                       Hardware::ButtonDebounceMs) {
                state_ = State::Released;
                update.releasedEdge = true;
            }
            break;

        case State::DebouncingReleaseLong:
            if (rawPressed) {
                state_ = State::LongHeld;
            } else if (elapsed(now, transitionStarted_) >=
                       Hardware::ButtonDebounceMs) {
                state_ = State::Released;
                update.releasedEdge = true;
            }
            break;
    }

    return update;
}

bool DebouncedButton::isPressed() const noexcept {
    return state_ != State::Released && state_ != State::DebouncingPress;
}

bool DebouncedButton::isLongHeld() const noexcept {
    return state_ == State::LongHeld ||
           state_ == State::DebouncingReleaseLong;
}

}  // namespace Mcm
