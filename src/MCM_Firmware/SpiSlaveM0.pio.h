/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// -------------------------------------------------- //
// This file is generated from SpiSlaveM0.pio.        //
// Commit it so Arduino IDE builds require no hook.   //
// -------------------------------------------------- //

#pragma once

#include <cstdint>

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

#define spi_slave_m0_msb_rx_wrap_target 0
#define spi_slave_m0_msb_rx_wrap 16
#define spi_slave_m0_msb_rx_pio_version 0

static const std::uint16_t spi_slave_m0_msb_rx_program_instructions[] = {
    0x200d,  //  0: wait 0 gpio 13
    0x200c,  //  1: wait 0 gpio 12
    0x8080,  //  2: pull noblock
    0x6001,  //  3: out  pins, 1
    0xa0c3,  //  4: mov  isr, null
    0xe027,  //  5: set  x, 7
    0x208c,  //  6: wait 1 gpio 12
    0x4001,  //  7: in   pins, 1
    0x200c,  //  8: wait 0 gpio 12
    0x6001,  //  9: out  pins, 1
    0x0046,  // 10: jmp  x--, 6
    0x8020,  // 11: push block
    0x00ce,  // 12: jmp  pin, 14
    0x0002,  // 13: jmp  2
    0xa0cb,  // 14: mov  isr, ~null
    0x8020,  // 15: push block
    0x0000,  // 16: jmp  0
};

#if !PICO_NO_HARDWARE
static const struct pio_program spi_slave_m0_msb_rx_program = {
    spi_slave_m0_msb_rx_program_instructions,
    17U,
    -1,
    spi_slave_m0_msb_rx_pio_version
#if PICO_PIO_VERSION > 0
    , 0x01U
#endif
};

static inline pio_sm_config
spi_slave_m0_msb_rx_program_get_default_config(uint offset) {
    pio_sm_config config = pio_get_default_sm_config();
    sm_config_set_wrap(
        &config,
        offset + spi_slave_m0_msb_rx_wrap_target,
        offset + spi_slave_m0_msb_rx_wrap);
    return config;
}
#endif
