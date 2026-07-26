/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 */

#pragma once

#include <cstdint>

inline void gpio_pull_up(std::uint8_t) noexcept {}
inline std::uint32_t gpio_get(std::uint8_t) noexcept { return 0U; }
