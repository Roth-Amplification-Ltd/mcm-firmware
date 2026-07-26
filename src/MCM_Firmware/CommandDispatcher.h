/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "Diagnostics.h"
#include "McmTypes.h"
#include "TransportProtocol.h"

#include <cstdint>

namespace Mcm {

class CommandDispatcher final {
public:
    enum class ActionType : std::uint8_t {
        None = 0U,
        RequestSnapshot,
        ResetControl,
        ResetAll,
        Resync,
        Unsupported,
        Invalid,
    };

    struct Action final {
        ActionType type = ActionType::None;
        ControlIndex index = 0U;
    };

    [[nodiscard]] Action dispatch(const Protocol::Packet& packet,
                                  Diagnostics& diagnostics) const noexcept;
};

}  // namespace Mcm
