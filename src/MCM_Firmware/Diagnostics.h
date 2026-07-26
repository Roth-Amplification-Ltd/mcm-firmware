/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include <cstdint>
#include <limits>

namespace Mcm {

struct Diagnostics final {
    std::uint32_t badSync = 0U;
    std::uint32_t badVersion = 0U;
    std::uint32_t badCrc = 0U;
    std::uint32_t shortFrame = 0U;
    std::uint32_t longFrame = 0U;
    std::uint32_t rxOverwrite = 0U;
    std::uint32_t eventQueueOverflow = 0U;
    std::uint32_t txQueueOverflow = 0U;
    std::uint32_t txUnderrun = 0U;
    std::uint32_t unsupportedCommand = 0U;
    std::uint32_t invalidControlIndex = 0U;
    std::uint32_t encoderInitFailure = 0U;
    std::uint32_t resyncCount = 0U;

    static void increment(std::uint32_t& counter) noexcept {
        if (counter < std::numeric_limits<std::uint32_t>::max()) {
            ++counter;
        }
    }
};

}  // namespace Mcm
