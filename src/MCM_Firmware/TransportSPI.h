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
#include "EventQueue.h"
#include "HardwareConfig.h"
#include "TransportProtocol.h"

#include <array>
#include <cstddef>
#include <cstdint>

#include <hardware/pio.h>

namespace Mcm {

class TransportSPI final {
public:
    enum class RxFrameState : std::uint8_t {
        Collecting = 0U,
        Overflow,
    };

    TransportSPI() noexcept = default;

    void begin() noexcept;
    void service(Diagnostics& diagnostics) noexcept;

    [[nodiscard]] bool enqueueTxPacket(const Protocol::Packet& packet,
                                       Diagnostics& diagnostics) noexcept;
    [[nodiscard]] bool hasRxPacket() const noexcept { return rxReady_; }
    [[nodiscard]] bool popRxPacket(Protocol::Packet& packet) noexcept;

    void clearTxQueue() noexcept;
    [[nodiscard]] bool integrityLost() const noexcept { return integrityLost_; }
    void clearIntegrityFault() noexcept { integrityLost_ = false; }
    [[nodiscard]] bool txIdle() const noexcept {
        return txQueue_.empty() && !txPacketLoaded_ && !txPacketStaged_;
    }

private:
    inline static constexpr std::uint32_t FrameEndMarker = 0xFFFFFFFFU;

    void processRxWord(std::uint32_t word, Diagnostics& diagnostics) noexcept;
    void completeRxFrame(Diagnostics& diagnostics) noexcept;
    void serviceTxFifo() noexcept;
    void retireTransmittedPacket(Diagnostics& diagnostics) noexcept;
    void setIrq(bool asserted) noexcept;
    void putTxByte(Byte byte) noexcept;

    PIO pio_ = pio1;
    std::uint8_t stateMachine_ = Hardware::SpiStateMachine;
    std::uint8_t programOffset_ = 0U;

    EventQueue<Protocol::Packet, Hardware::SpiTxQueueDepth> txQueue_{};
    Protocol::Packet currentTxPacket_{};
    std::size_t txFeedIndex_ = 0U;
    bool txPacketLoaded_ = false;
    bool txPacketStaged_ = false;

    Protocol::Packet rxAssembly_{};
    Protocol::Packet rxPacket_{};
    std::size_t rxIndex_ = 0U;
    RxFrameState rxFrameState_ = RxFrameState::Collecting;
    bool rxReady_ = false;
    bool integrityLost_ = false;
};

}  // namespace Mcm
