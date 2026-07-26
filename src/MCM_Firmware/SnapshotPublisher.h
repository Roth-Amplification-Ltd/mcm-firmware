/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "ControlState.h"
#include "EventQueue.h"
#include "HardwareConfig.h"
#include "McmTypes.h"
#include "TransportProtocol.h"

#include <cstddef>
#include <cstdint>

namespace Mcm {

class SnapshotPublisher final {
public:
    enum class SnapshotPhase : std::uint8_t {
        Idle = 0U,
        Begin,
        Encoders,
        Buttons,
        End,
    };

    SnapshotPublisher() noexcept = default;

    [[nodiscard]] bool queueEncoder(ControlIndex index,
                                    EncoderValue value) noexcept;
    [[nodiscard]] bool queueButtons(Byte pressedBitmap,
                                    Byte longHeldBitmap) noexcept;
    [[nodiscard]] bool requestSnapshot(const ControlSnapshot& snapshot) noexcept;

    void service() noexcept;
    [[nodiscard]] bool hasPacket() const noexcept { return packetReady_; }
    [[nodiscard]] const Protocol::Packet& packet() const noexcept {
        return packet_;
    }
    void consumePacket() noexcept { packetReady_ = false; }
    void clear() noexcept;

    [[nodiscard]] bool snapshotInProgress() const noexcept {
        return snapshotPhase_ != SnapshotPhase::Idle || packetReadySnapshotEnd_;
    }
    [[nodiscard]] bool idle() const noexcept {
        return !packetReady_ && !packetReadySnapshotEnd_ &&
               snapshotPhase_ == SnapshotPhase::Idle && events_.empty();
    }

private:
    enum class EventType : std::uint8_t {
        Encoder = 0U,
        Buttons,
    };

    struct PublicationEvent final {
        EventType type = EventType::Encoder;
        ControlIndex index = 0U;
        EncoderValue value = 0;
        Byte pressedBitmap = 0U;
        Byte longHeldBitmap = 0U;
    };

    void buildSnapshotPacket() noexcept;
    void buildEventPacket(const PublicationEvent& event) noexcept;

    EventQueue<PublicationEvent, Hardware::PublicationQueueDepth> events_{};
    ControlSnapshot snapshot_{};
    SnapshotPhase snapshotPhase_ = SnapshotPhase::Idle;
    std::size_t snapshotEncoderIndex_ = 0U;
    Protocol::Packet packet_{};
    bool packetReady_ = false;
    bool packetReadySnapshotEnd_ = false;
};

}  // namespace Mcm
