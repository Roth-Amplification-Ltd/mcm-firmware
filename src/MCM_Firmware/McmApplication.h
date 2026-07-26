/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "CommandDispatcher.h"
#include "ControlScanner.h"
#include "Diagnostics.h"
#include "McmTypes.h"
#include "SnapshotPublisher.h"
#include "TransportSPI.h"

#include <cstdint>

namespace Mcm {

class McmApplication final {
public:
    enum class State : std::uint8_t {
        Starting = 0U,
        Running,
        Resynchronizing,
        Fault,
    };

    McmApplication() noexcept = default;

    void begin() noexcept;
    void service() noexcept;

    [[nodiscard]] State state() const noexcept { return state_; }
    [[nodiscard]] const Diagnostics& diagnostics() const noexcept {
        return diagnostics_;
    }

private:
    void handleCommand(const CommandDispatcher::Action& action) noexcept;
    void publishChanges(const ControlChanges& changes) noexcept;
    void requestFreshSnapshot() noexcept;
    void recoverPublicationIntegrity() noexcept;
    void transferOnePublishedPacket() noexcept;
    void updateRecoveryState() noexcept;

    Diagnostics diagnostics_{};
    ControlScanner scanner_{};
    SnapshotPublisher publisher_{};
    CommandDispatcher dispatcher_{};
    TransportSPI transport_{};
    State state_ = State::Starting;
    Byte snapshotSequence_ = 0U;
};

}  // namespace Mcm
