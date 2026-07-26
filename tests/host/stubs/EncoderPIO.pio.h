/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 */

#pragma once

#include <hardware/pio.h>

inline const pio_program encoder_state_stream_program{};
inline pio_sm_config encoder_state_stream_program_get_default_config(uint) noexcept {
    return {};
}
