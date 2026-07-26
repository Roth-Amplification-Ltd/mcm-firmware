/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "CommandDispatcher.h"
#include "ControlState.h"
#include "DebouncedButton.h"
#include "Diagnostics.h"
#include "EventQueue.h"
#include "SnapshotPublisher.h"
#include "TransportProtocol.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {

using Mcm::Protocol::MessageType;
using Mcm::Protocol::Packet;

void testCrcAndSigned24() {
    constexpr Packet command =
        Mcm::Protocol::makeCommand(MessageType::CommandGetSnapshot);
    static_assert(command[0U] == 0xA5U);
    static_assert(command[1U] == 0x01U);
    static_assert(Mcm::Protocol::validate(command) ==
                  Mcm::Protocol::ValidationResult::Valid);

    constexpr Packet negative = Mcm::Protocol::makeEncoderState(2U, -12345);
    static_assert(Mcm::Protocol::readSigned24(negative) == -12345);
    static_assert(Mcm::Protocol::validate(negative) ==
                  Mcm::Protocol::ValidationResult::Valid);

    Packet corrupt = negative;
    corrupt[4U] = static_cast<std::uint8_t>(corrupt[4U] ^ 0x01U);
    assert(Mcm::Protocol::validate(corrupt) ==
           Mcm::Protocol::ValidationResult::BadCrc);
}

void testFixedQueue() {
    Mcm::EventQueue<std::uint8_t, 2U> queue;
    assert(queue.push(10U));
    assert(queue.push(20U));
    assert(!queue.push(30U));

    std::uint8_t value = 0U;
    assert(queue.pop(value));
    assert(value == 10U);
    assert(queue.pop(value));
    assert(value == 20U);
    assert(!queue.pop(value));
}

void testCommandDispatcher() {
    Mcm::Diagnostics diagnostics{};
    Mcm::CommandDispatcher dispatcher{};
    const Packet command =
        Mcm::Protocol::makeCommand(MessageType::CommandResetControl, 4U);
    const Mcm::CommandDispatcher::Action action =
        dispatcher.dispatch(command, diagnostics);
    assert(action.type == Mcm::CommandDispatcher::ActionType::ResetControl);
    assert(action.index == 4U);
}

void testButtonStateMachine() {
    Mcm::DebouncedButton button{};

    Mcm::ButtonUpdate update = button.serviceRaw(true, 0U);
    assert(!update.pressedEdge);
    update = button.serviceRaw(true, Mcm::Hardware::ButtonDebounceMs);
    assert(update.pressedEdge);
    assert(button.isPressed());

    update = button.serviceRaw(
        true,
        Mcm::Hardware::ButtonDebounceMs + Mcm::Hardware::ButtonLongPressMs);
    assert(update.longHoldEdge);
    assert(button.isLongHeld());

    update = button.serviceRaw(false, 700U);
    assert(!update.releasedEdge);
    update = button.serviceRaw(false, 700U + Mcm::Hardware::ButtonDebounceMs);
    assert(update.releasedEdge);
    assert(!button.isPressed());
}

void testAtomicSnapshotSequence() {
    Mcm::ControlSnapshot snapshot{};
    snapshot.encoderValues = {{11, 22, 33, 44, 55, 66}};
    snapshot.pressedBitmap = 0x15U;
    snapshot.longHeldBitmap = 0x04U;
    snapshot.sequence = 7U;

    Mcm::SnapshotPublisher publisher{};
    assert(publisher.requestSnapshot(snapshot));

    std::array<Packet, Mcm::Hardware::SnapshotPacketCount> packets{};
    for (std::size_t index = 0U; index < packets.size(); ++index) {
        publisher.service();
        assert(publisher.hasPacket());
        packets[index] = publisher.packet();
        publisher.consumePacket();
    }

    publisher.service();
    assert(publisher.idle());
    assert(packets[0U][2U] == static_cast<std::uint8_t>(MessageType::SnapshotBegin));
    for (std::size_t index = 0U; index < Mcm::ControlCount; ++index) {
        const Packet& packet = packets[index + 1U];
        assert(packet[2U] == static_cast<std::uint8_t>(MessageType::EncoderState));
        assert(packet[3U] == static_cast<std::uint8_t>(index));
        assert(Mcm::Protocol::readSigned24(packet) == snapshot.encoderValues[index]);
        assert(Mcm::Protocol::validate(packet) ==
               Mcm::Protocol::ValidationResult::Valid);
    }

    const Packet& buttons = packets[7U];
    assert(buttons[2U] == static_cast<std::uint8_t>(MessageType::ButtonState));
    assert(buttons[4U] == snapshot.pressedBitmap);
    assert(buttons[5U] == snapshot.longHeldBitmap);
    assert(buttons[6U] == Mcm::ControlCount);
    assert(packets[8U][2U] == static_cast<std::uint8_t>(MessageType::SnapshotEnd));
    assert(packets[8U][4U] == snapshot.sequence);
}

}  // namespace

int main() {
    testCrcAndSigned24();
    testFixedQueue();
    testCommandDispatcher();
    testButtonStateMachine();
    testAtomicSnapshotSequence();
    std::cout << "MCM host protocol tests passed.\n";
    return 0;
}
