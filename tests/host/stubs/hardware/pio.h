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

#ifndef PICO_NO_HARDWARE
#define PICO_NO_HARDWARE 0
#endif

#ifndef PICO_PIO_VERSION
#define PICO_PIO_VERSION 0
#endif

using uint = unsigned int;

struct pio_hw_t final {};
using PIO = pio_hw_t*;
inline pio_hw_t pio0_instance{};
inline pio_hw_t pio1_instance{};
inline PIO pio0 = &pio0_instance;
inline PIO pio1 = &pio1_instance;

struct pio_program final {
    const std::uint16_t* instructions;
    std::uint8_t length;
    std::int8_t origin;
    std::uint8_t pio_version;
#if PICO_PIO_VERSION > 0
    std::uint8_t used_gpio_ranges;
#endif
};

struct pio_sm_config final {};

inline pio_sm_config pio_get_default_sm_config() noexcept { return {}; }
inline void sm_config_set_wrap(pio_sm_config*, uint, uint) noexcept {}
inline uint pio_add_program(PIO, const pio_program*) noexcept { return 0U; }
inline void pio_gpio_init(PIO, uint) noexcept {}
inline void pio_sm_set_consecutive_pindirs(PIO, uint, uint, uint, bool) noexcept {}
inline void sm_config_set_in_pins(pio_sm_config*, uint) noexcept {}
inline void sm_config_set_out_pins(pio_sm_config*, uint, uint) noexcept {}
inline void sm_config_set_jmp_pin(pio_sm_config*, uint) noexcept {}
inline void sm_config_set_out_shift(pio_sm_config*, bool, bool, uint) noexcept {}
inline void sm_config_set_in_shift(pio_sm_config*, bool, bool, uint) noexcept {}
inline void sm_config_set_clkdiv(pio_sm_config*, float) noexcept {}
inline void pio_sm_init(PIO, uint, uint, const pio_sm_config*) noexcept {}
inline void pio_sm_clear_fifos(PIO, uint) noexcept {}
inline void pio_sm_set_enabled(PIO, uint, bool) noexcept {}
inline bool pio_sm_is_rx_fifo_empty(PIO, uint) noexcept { return true; }
inline std::uint32_t pio_sm_get(PIO, uint) noexcept { return 0U; }
inline bool pio_sm_is_tx_fifo_full(PIO, uint) noexcept { return false; }
inline void pio_sm_put(PIO, uint, std::uint32_t) noexcept {}
inline void pio_sm_restart(PIO, uint) noexcept {}
