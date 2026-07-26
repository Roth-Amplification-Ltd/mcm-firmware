/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "CommandDispatcher.h"

namespace Mcm {

CommandDispatcher::Action CommandDispatcher::dispatch(
    const Protocol::Packet& packet,
    Diagnostics& diagnostics) const noexcept {
    const Protocol::ValidationResult validation = Protocol::validate(packet);

    switch (validation) {
        case Protocol::ValidationResult::BadSync:
            Diagnostics::increment(diagnostics.badSync);
            return {ActionType::Invalid, 0U};
        case Protocol::ValidationResult::BadVersion:
            Diagnostics::increment(diagnostics.badVersion);
            return {ActionType::Invalid, 0U};
        case Protocol::ValidationResult::BadCrc:
            Diagnostics::increment(diagnostics.badCrc);
            return {ActionType::Invalid, 0U};
        case Protocol::ValidationResult::Valid:
            break;
    }

    const auto type = static_cast<Protocol::MessageType>(
        packet[Protocol::TypeOffset]);
    const ControlIndex index = packet[Protocol::IndexOffset];

    switch (type) {
        case Protocol::MessageType::CommandNop:
            return {ActionType::None, 0U};
        case Protocol::MessageType::CommandGetSnapshot:
            return {ActionType::RequestSnapshot, 0U};
        case Protocol::MessageType::CommandResetControl:
            return {ActionType::ResetControl, index};
        case Protocol::MessageType::CommandResetAll:
            return {ActionType::ResetAll, 0U};
        case Protocol::MessageType::CommandResync:
            return {ActionType::Resync, 0U};
        case Protocol::MessageType::CommandSetLed:
        case Protocol::MessageType::CommandGetInfo:
        case Protocol::MessageType::EncoderState:
        case Protocol::MessageType::ButtonState:
        case Protocol::MessageType::SnapshotBegin:
        case Protocol::MessageType::SnapshotEnd:
            Diagnostics::increment(diagnostics.unsupportedCommand);
            return {ActionType::Unsupported, index};
    }

    Diagnostics::increment(diagnostics.unsupportedCommand);
    return {ActionType::Unsupported, index};
}

}  // namespace Mcm
