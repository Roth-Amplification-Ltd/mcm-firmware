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

#include <array>
#include <cstdint>

namespace Mcm {

struct ControlSnapshot final {
    std::array<EncoderValue, ControlCount> encoderValues{};
    Byte pressedBitmap = 0U;
    Byte longHeldBitmap = 0U;
    Byte sequence = 0U;
};

struct ControlChanges final {
    Byte encoderChangedMask = 0U;
    bool buttonStateChanged = false;

    [[nodiscard]] bool any() const noexcept {
        return (encoderChangedMask != 0U) || buttonStateChanged;
    }
};

}  // namespace Mcm
