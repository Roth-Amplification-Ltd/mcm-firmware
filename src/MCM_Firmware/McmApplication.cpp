/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "McmApplication.h"

#include <Arduino.h>

#include <cstddef>

namespace Mcm {
namespace {

[[nodiscard]] Byte bitForIndex(std::size_t index) noexcept {
    return static_cast<Byte>(1U << static_cast<unsigned int>(index));
}

}  // namespace

void McmApplication::begin() noexcept {
    Serial.begin(Hardware::SerialBaud);
    transport_.begin();

    const Milliseconds now = static_cast<Milliseconds>(millis());
    if (!scanner_.begin(now, diagnostics_)) {
        state_ = State::Fault;
        return;
    }

    state_ = State::Resynchronizing;
    requestFreshSnapshot();
}

void McmApplication::service() noexcept {
    transport_.service(diagnostics_);
    if (transport_.integrityLost()) {
        transport_.clearIntegrityFault();
        recoverPublicationIntegrity();
    }

    Protocol::Packet command{};
    if (transport_.popRxPacket(command)) {
        handleCommand(dispatcher_.dispatch(command, diagnostics_));
    }

    if (state_ != State::Fault) {
        const Milliseconds now = static_cast<Milliseconds>(millis());
        publishChanges(scanner_.service(now));
        publisher_.service();
        transferOnePublishedPacket();
        updateRecoveryState();
    }
}

void McmApplication::handleCommand(
    const CommandDispatcher::Action& action) noexcept {
    switch (action.type) {
        case CommandDispatcher::ActionType::None:
        case CommandDispatcher::ActionType::Unsupported:
        case CommandDispatcher::ActionType::Invalid:
            break;

        case CommandDispatcher::ActionType::RequestSnapshot:
            requestFreshSnapshot();
            break;

        case CommandDispatcher::ActionType::ResetControl:
            if (!scanner_.resetControl(action.index)) {
                Diagnostics::increment(diagnostics_.invalidControlIndex);
            } else if (!publisher_.queueEncoder(
                           action.index,
                           scanner_.encoderValue(action.index))) {
                recoverPublicationIntegrity();
            }
            break;

        case CommandDispatcher::ActionType::ResetAll:
            scanner_.resetAll();
            requestFreshSnapshot();
            break;

        case CommandDispatcher::ActionType::Resync:
            recoverPublicationIntegrity();
            break;
    }
}

void McmApplication::publishChanges(const ControlChanges& changes) noexcept {
    for (std::size_t index = 0U; index < ControlCount; ++index) {
        const Byte mask = bitForIndex(index);
        if ((changes.encoderChangedMask & mask) != 0U) {
            const ControlIndex control = static_cast<ControlIndex>(index);
            if (!publisher_.queueEncoder(control,
                                         scanner_.encoderValue(control))) {
                Diagnostics::increment(diagnostics_.eventQueueOverflow);
                recoverPublicationIntegrity();
                return;
            }
        }
    }

    if (changes.buttonStateChanged) {
        const ControlSnapshot snapshot = scanner_.captureSnapshot(snapshotSequence_);
        if (!publisher_.queueButtons(snapshot.pressedBitmap,
                                     snapshot.longHeldBitmap)) {
            Diagnostics::increment(diagnostics_.eventQueueOverflow);
            recoverPublicationIntegrity();
        }
    }
}

void McmApplication::requestFreshSnapshot() noexcept {
    if (state_ == State::Fault) {
        return;
    }

    snapshotSequence_ = static_cast<Byte>(snapshotSequence_ + 1U);
    const ControlSnapshot snapshot = scanner_.captureSnapshot(snapshotSequence_);

    if (!publisher_.requestSnapshot(snapshot)) {
        recoverPublicationIntegrity();
        return;
    }

    state_ = State::Resynchronizing;
}

void McmApplication::recoverPublicationIntegrity() noexcept {
    if (state_ == State::Fault) {
        return;
    }

    Diagnostics::increment(diagnostics_.resyncCount);
    transport_.clearTxQueue();
    publisher_.clear();
    state_ = State::Resynchronizing;

    snapshotSequence_ = static_cast<Byte>(snapshotSequence_ + 1U);
    const ControlSnapshot snapshot = scanner_.captureSnapshot(snapshotSequence_);
    if (!publisher_.requestSnapshot(snapshot)) {
        state_ = State::Fault;
    }
}

void McmApplication::transferOnePublishedPacket() noexcept {
    if (!publisher_.hasPacket()) {
        return;
    }

    if (transport_.enqueueTxPacket(publisher_.packet(), diagnostics_)) {
        publisher_.consumePacket();
    }
}

void McmApplication::updateRecoveryState() noexcept {
    if (state_ == State::Resynchronizing && publisher_.idle() &&
        transport_.txIdle()) {
        state_ = State::Running;
    }
}

}  // namespace Mcm
