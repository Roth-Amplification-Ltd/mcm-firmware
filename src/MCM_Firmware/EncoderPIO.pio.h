/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// -------------------------------------------------- //
// This file is generated from EncoderPIO.pio.        //
// Commit it so Arduino IDE builds require no hook.   //
// -------------------------------------------------- //

#pragma once

#include <cstdint>

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

#define encoder_state_stream_wrap_target 0
#define encoder_state_stream_wrap 11
#define encoder_state_stream_pio_version 0

static const std::uint16_t encoder_state_stream_program_instructions[] = {
    0xa0c3,  //  0: mov isr, null
    0x4002,  //  1: in  pins, 2
    0xa026,  //  2: mov x, isr
    0xa0c3,  //  3: mov isr, null
    0x4002,  //  4: in  pins, 2
    0xa046,  //  5: mov y, isr
    0x00a8,  //  6: jmp x!=y, 8
    0x0003,  //  7: jmp 3
    0xa022,  //  8: mov x, y
    0xa0c2,  //  9: mov isr, y
    0x8020,  // 10: push block
    0x0003,  // 11: jmp 3
};

#if !PICO_NO_HARDWARE
static const struct pio_program encoder_state_stream_program = {
    encoder_state_stream_program_instructions,
    12U,
    -1,
    encoder_state_stream_pio_version
#if PICO_PIO_VERSION > 0
    , 0x0U
#endif
};

static inline pio_sm_config
encoder_state_stream_program_get_default_config(uint offset) {
    pio_sm_config config = pio_get_default_sm_config();
    sm_config_set_wrap(
        &config,
        offset + encoder_state_stream_wrap_target,
        offset + encoder_state_stream_wrap);
    return config;
}
#endif
