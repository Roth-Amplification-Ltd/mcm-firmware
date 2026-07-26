/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "McmTypes.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace Mcm::Protocol {

inline constexpr Byte SyncByte = 0xA5U;
inline constexpr Byte Version = 0x01U;
inline constexpr Byte CrcPolynomial = 0x07U;

inline constexpr std::size_t SyncOffset = 0U;
inline constexpr std::size_t VersionOffset = 1U;
inline constexpr std::size_t TypeOffset = 2U;
inline constexpr std::size_t IndexOffset = 3U;
inline constexpr std::size_t Data0Offset = 4U;
inline constexpr std::size_t Data1Offset = 5U;
inline constexpr std::size_t Data2Offset = 6U;
inline constexpr std::size_t CrcOffset = 7U;

inline constexpr EncoderValue Signed24Minimum = -8388608;
inline constexpr EncoderValue Signed24Maximum = 8388607;

using Packet = std::array<Byte, PacketLength>;

enum class MessageType : Byte {
    EncoderState = 0x01U,
    ButtonState = 0x02U,
    SnapshotBegin = 0x10U,
    SnapshotEnd = 0x11U,
    CommandNop = 0x80U,
    CommandGetSnapshot = 0x81U,
    CommandResetControl = 0x82U,
    CommandResetAll = 0x83U,
    CommandSetLed = 0x84U,
    CommandGetInfo = 0x85U,
    CommandResync = 0x86U,
};

enum class ValidationResult : Byte {
    Valid = 0U,
    BadSync,
    BadVersion,
    BadCrc,
};

static_assert(Packet{}.size() == PacketLength,
              "Packet storage must exactly match the wire envelope.");
static_assert(Signed24Minimum >= std::numeric_limits<EncoderValue>::min(),
              "EncoderValue must represent signed 24-bit minimum.");
static_assert(Signed24Maximum <= std::numeric_limits<EncoderValue>::max(),
              "EncoderValue must represent signed 24-bit maximum.");

[[nodiscard]] constexpr Byte crc8(const Byte* data, std::size_t length) noexcept {
    Byte crc = 0U;

    for (std::size_t index = 0U; index < length; ++index) {
        crc = static_cast<Byte>(crc ^ data[index]);

        for (std::uint8_t bit = 0U; bit < 8U; ++bit) {
            if ((crc & 0x80U) != 0U) {
                crc = static_cast<Byte>(
                    static_cast<Byte>(crc << 1U) ^ CrcPolynomial);
            } else {
                crc = static_cast<Byte>(crc << 1U);
            }
        }
    }

    return crc;
}

[[nodiscard]] constexpr Byte packetCrc(const Packet& packet) noexcept {
    return crc8(packet.data(), CrcOffset);
}

[[nodiscard]] constexpr EncoderValue clampSigned24(EncoderValue value) noexcept {
    return (value < Signed24Minimum)
               ? Signed24Minimum
               : ((value > Signed24Maximum) ? Signed24Maximum : value);
}

constexpr void writeSigned24(Packet& packet, EncoderValue value) noexcept {
    const std::uint32_t encoded = static_cast<std::uint32_t>(clampSigned24(value));
    packet[Data0Offset] = static_cast<Byte>(encoded & 0xFFU);
    packet[Data1Offset] = static_cast<Byte>((encoded >> 8U) & 0xFFU);
    packet[Data2Offset] = static_cast<Byte>((encoded >> 16U) & 0xFFU);
}

[[nodiscard]] constexpr EncoderValue readSigned24(const Packet& packet) noexcept {
    std::uint32_t encoded = static_cast<std::uint32_t>(packet[Data0Offset]);
    encoded |= static_cast<std::uint32_t>(packet[Data1Offset]) << 8U;
    encoded |= static_cast<std::uint32_t>(packet[Data2Offset]) << 16U;

    if ((encoded & 0x00800000U) != 0U) {
        return static_cast<EncoderValue>(encoded) - 0x01000000;
    }

    return static_cast<EncoderValue>(encoded);
}

[[nodiscard]] constexpr Packet makeBasePacket(MessageType type,
                                               Byte index) noexcept {
    Packet packet{};
    packet[SyncOffset] = SyncByte;
    packet[VersionOffset] = Version;
    packet[TypeOffset] = static_cast<Byte>(type);
    packet[IndexOffset] = index;
    return packet;
}

[[nodiscard]] constexpr Packet makeEncoderState(ControlIndex index,
                                                EncoderValue value) noexcept {
    Packet packet = makeBasePacket(MessageType::EncoderState, index);
    writeSigned24(packet, value);
    packet[CrcOffset] = packetCrc(packet);
    return packet;
}

[[nodiscard]] constexpr Packet makeButtonState(Byte pressedBitmap,
                                               Byte longHeldBitmap) noexcept {
    Packet packet = makeBasePacket(MessageType::ButtonState,
                                   static_cast<Byte>(ControlCount));
    packet[Data0Offset] = static_cast<Byte>(pressedBitmap & ValidControlMask);
    packet[Data1Offset] = static_cast<Byte>(longHeldBitmap & ValidControlMask);
    packet[Data2Offset] = static_cast<Byte>(ControlCount);
    packet[CrcOffset] = packetCrc(packet);
    return packet;
}

[[nodiscard]] constexpr Packet makeSnapshotBoundary(MessageType type,
                                                    Byte sequence) noexcept {
    Packet packet = makeBasePacket(type, static_cast<Byte>(ControlCount));
    packet[Data0Offset] = sequence;
    packet[Data1Offset] = 0U;
    packet[Data2Offset] = 0U;
    packet[CrcOffset] = packetCrc(packet);
    return packet;
}

[[nodiscard]] constexpr Packet makeCommand(MessageType type,
                                           Byte index = 0U) noexcept {
    Packet packet = makeBasePacket(type, index);
    packet[CrcOffset] = packetCrc(packet);
    return packet;
}

[[nodiscard]] constexpr ValidationResult validate(const Packet& packet) noexcept {
    if (packet[SyncOffset] != SyncByte) {
        return ValidationResult::BadSync;
    }
    if (packet[VersionOffset] != Version) {
        return ValidationResult::BadVersion;
    }
    if (packet[CrcOffset] != packetCrc(packet)) {
        return ValidationResult::BadCrc;
    }
    return ValidationResult::Valid;
}

}  // namespace Mcm::Protocol
