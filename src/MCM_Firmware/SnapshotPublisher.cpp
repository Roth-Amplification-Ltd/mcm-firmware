/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "SnapshotPublisher.h"

#include <cstddef>

namespace Mcm {

bool SnapshotPublisher::queueEncoder(ControlIndex index,
                                     EncoderValue value) noexcept {
    const PublicationEvent event{
        EventType::Encoder,
        index,
        value,
        0U,
        0U,
    };
    return events_.push(event);
}

bool SnapshotPublisher::queueButtons(Byte pressedBitmap,
                                     Byte longHeldBitmap) noexcept {
    const PublicationEvent event{
        EventType::Buttons,
        0U,
        0,
        pressedBitmap,
        longHeldBitmap,
    };
    return events_.push(event);
}

bool SnapshotPublisher::requestSnapshot(
    const ControlSnapshot& snapshot) noexcept {
    if (snapshotPhase_ != SnapshotPhase::Idle || packetReady_ ||
        packetReadySnapshotEnd_) {
        return false;
    }

    snapshot_ = snapshot;
    snapshotEncoderIndex_ = 0U;
    snapshotPhase_ = SnapshotPhase::Begin;
    return true;
}

void SnapshotPublisher::service() noexcept {
    if (packetReady_) {
        return;
    }

    if (packetReadySnapshotEnd_) {
        packetReadySnapshotEnd_ = false;
    }

    if (snapshotPhase_ != SnapshotPhase::Idle) {
        buildSnapshotPacket();
        packetReady_ = true;
        return;
    }

    PublicationEvent event{};
    if (events_.pop(event)) {
        buildEventPacket(event);
        packetReady_ = true;
    }
}

void SnapshotPublisher::clear() noexcept {
    events_.clear();
    snapshot_ = ControlSnapshot{};
    snapshotPhase_ = SnapshotPhase::Idle;
    snapshotEncoderIndex_ = 0U;
    packet_ = Protocol::Packet{};
    packetReady_ = false;
    packetReadySnapshotEnd_ = false;
}

void SnapshotPublisher::buildSnapshotPacket() noexcept {
    switch (snapshotPhase_) {
        case SnapshotPhase::Idle:
            break;

        case SnapshotPhase::Begin:
            packet_ = Protocol::makeSnapshotBoundary(
                Protocol::MessageType::SnapshotBegin,
                snapshot_.sequence);
            snapshotPhase_ = SnapshotPhase::Encoders;
            break;

        case SnapshotPhase::Encoders:
            packet_ = Protocol::makeEncoderState(
                static_cast<ControlIndex>(snapshotEncoderIndex_),
                snapshot_.encoderValues[snapshotEncoderIndex_]);
            ++snapshotEncoderIndex_;
            if (snapshotEncoderIndex_ >= ControlCount) {
                snapshotPhase_ = SnapshotPhase::Buttons;
            }
            break;

        case SnapshotPhase::Buttons:
            packet_ = Protocol::makeButtonState(snapshot_.pressedBitmap,
                                                snapshot_.longHeldBitmap);
            snapshotPhase_ = SnapshotPhase::End;
            break;

        case SnapshotPhase::End:
            packet_ = Protocol::makeSnapshotBoundary(
                Protocol::MessageType::SnapshotEnd,
                snapshot_.sequence);
            snapshotPhase_ = SnapshotPhase::Idle;
            packetReadySnapshotEnd_ = true;
            break;
    }
}

void SnapshotPublisher::buildEventPacket(
    const PublicationEvent& event) noexcept {
    switch (event.type) {
        case EventType::Encoder:
            packet_ = Protocol::makeEncoderState(event.index, event.value);
            break;
        case EventType::Buttons:
            packet_ = Protocol::makeButtonState(event.pressedBitmap,
                                                event.longHeldBitmap);
            break;
    }
}

}  // namespace Mcm
