/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "ControlScanner.h"

#include <cstddef>
#include <cstdint>

#include <hardware/pio.h>

namespace Mcm {
namespace {

[[nodiscard]] PIO pioForIndex(std::uint8_t index) noexcept {
    return (index == 0U) ? pio0 : pio1;
}

[[nodiscard]] Byte bitForIndex(std::size_t index) noexcept {
    return static_cast<Byte>(1U << static_cast<unsigned int>(index));
}

}  // namespace

bool ControlScanner::begin(Milliseconds now,
                           Diagnostics& diagnostics) noexcept {
    bool initialized = true;

    for (std::size_t index = 0U; index < ControlCount; ++index) {
        const Hardware::ControlHardware& hardware = Hardware::Controls[index];
        const bool encoderReady = encoders_[index].begin(
            pioForIndex(hardware.pioIndex),
            hardware.stateMachine,
            hardware.encoderAPin,
            hardware.encoderBPin);

        if (!encoderReady) {
            Diagnostics::increment(diagnostics.encoderInitFailure);
            initialized = false;
        }

        buttons_[index].begin(hardware.buttonPin, now);
        encoderValues_[index] = encoders_[index].detentCount();
    }

    refreshButtonBitmaps();
    return initialized;
}

ControlChanges ControlScanner::service(Milliseconds now) noexcept {
    ControlChanges changes{};
    const Byte previousPressed = pressedBitmap_;
    const Byte previousLongHeld = longHeldBitmap_;

    for (std::size_t index = 0U; index < ControlCount; ++index) {
        encoders_[index].service();
        const EncoderValue value = encoders_[index].detentCount();

        if (value != encoderValues_[index]) {
            encoderValues_[index] = value;
            changes.encoderChangedMask = static_cast<Byte>(
                changes.encoderChangedMask | bitForIndex(index));
        }

        const ButtonUpdate buttonUpdate = buttons_[index].service(now);
        if (buttonUpdate.pressedEdge || buttonUpdate.releasedEdge ||
            buttonUpdate.longHoldEdge) {
            changes.buttonStateChanged = true;
        }
    }

    refreshButtonBitmaps();
    if ((pressedBitmap_ != previousPressed) ||
        (longHeldBitmap_ != previousLongHeld)) {
        changes.buttonStateChanged = true;
    }

    return changes;
}

ControlSnapshot ControlScanner::captureSnapshot(Byte sequence) const noexcept {
    ControlSnapshot snapshot{};
    snapshot.encoderValues = encoderValues_;
    snapshot.pressedBitmap = pressedBitmap_;
    snapshot.longHeldBitmap = longHeldBitmap_;
    snapshot.sequence = sequence;
    return snapshot;
}

EncoderValue ControlScanner::encoderValue(ControlIndex index) const noexcept {
    return isValidControlIndex(index)
               ? encoderValues_[static_cast<std::size_t>(index)]
               : 0;
}

bool ControlScanner::resetControl(ControlIndex index) noexcept {
    if (!isValidControlIndex(index)) {
        return false;
    }

    const std::size_t arrayIndex = static_cast<std::size_t>(index);
    encoders_[arrayIndex].setDetentCount(0);
    encoderValues_[arrayIndex] = 0;
    return true;
}

void ControlScanner::resetAll() noexcept {
    for (std::size_t index = 0U; index < ControlCount; ++index) {
        encoders_[index].setDetentCount(0);
        encoderValues_[index] = 0;
    }
}

void ControlScanner::refreshButtonBitmaps() noexcept {
    Byte pressed = 0U;
    Byte longHeld = 0U;

    for (std::size_t index = 0U; index < ControlCount; ++index) {
        const Byte mask = bitForIndex(index);
        if (buttons_[index].isPressed()) {
            pressed = static_cast<Byte>(pressed | mask);
        }
        if (buttons_[index].isLongHeld()) {
            longHeld = static_cast<Byte>(longHeld | mask);
        }
    }

    pressedBitmap_ = static_cast<Byte>(pressed & ValidControlMask);
    longHeldBitmap_ = static_cast<Byte>(longHeld & ValidControlMask);
}

}  // namespace Mcm
