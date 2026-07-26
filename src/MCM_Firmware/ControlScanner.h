/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "ControlMap.h"
#include "ControlState.h"
#include "DebouncedButton.h"
#include "Diagnostics.h"
#include "EncoderPIO.h"
#include "McmTypes.h"

#include <array>
#include <cstddef>

namespace Mcm {

class ControlScanner final {
public:
    ControlScanner() noexcept = default;

    [[nodiscard]] bool begin(Milliseconds now, Diagnostics& diagnostics) noexcept;
    [[nodiscard]] ControlChanges service(Milliseconds now) noexcept;

    [[nodiscard]] ControlSnapshot captureSnapshot(Byte sequence) const noexcept;
    [[nodiscard]] EncoderValue encoderValue(ControlIndex index) const noexcept;

    [[nodiscard]] bool resetControl(ControlIndex index) noexcept;
    void resetAll() noexcept;

private:
    void refreshButtonBitmaps() noexcept;

    std::array<EncoderPIO, ControlCount> encoders_{};
    std::array<DebouncedButton, ControlCount> buttons_{};
    std::array<EncoderValue, ControlCount> encoderValues_{};
    Byte pressedBitmap_ = 0U;
    Byte longHeldBitmap_ = 0U;
};

}  // namespace Mcm
