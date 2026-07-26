/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 */

#pragma once

#include <cstdint>

inline constexpr int LOW = 0;
inline constexpr int HIGH = 1;
inline constexpr int INPUT = 0;
inline constexpr int OUTPUT = 1;
inline constexpr int INPUT_PULLUP = 2;

class SerialStub final {
public:
    void begin(std::uint32_t) noexcept {}
};

inline SerialStub Serial{};

inline void pinMode(std::uint8_t, int) noexcept {}
inline int digitalRead(std::uint8_t) noexcept { return HIGH; }
inline void digitalWrite(std::uint8_t, int) noexcept {}
inline std::uint32_t millis() noexcept { return 0U; }
