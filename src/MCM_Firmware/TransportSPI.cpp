/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "TransportSPI.h"

#include "SpiSlaveM0.pio.h"

#include <Arduino.h>

#include <cstddef>
#include <cstdint>

#include <hardware/pio.h>

namespace Mcm {

void TransportSPI::begin() noexcept {
    pinMode(Hardware::SpiSckPin, INPUT);
    pinMode(Hardware::SpiCsPin, INPUT_PULLUP);
    pinMode(Hardware::SpiMosiPin, INPUT);
    pinMode(Hardware::SpiIrqPin, OUTPUT);
    setIrq(false);

    programOffset_ = static_cast<std::uint8_t>(
        pio_add_program(pio_, &spi_slave_m0_msb_rx_program));

    pio_gpio_init(pio_, Hardware::SpiMisoPin);
    pio_sm_set_consecutive_pindirs(
        pio_, stateMachine_, Hardware::SpiMisoPin, 1U, true);

    pio_sm_config config =
        spi_slave_m0_msb_rx_program_get_default_config(programOffset_);
    sm_config_set_in_pins(&config, Hardware::SpiMosiPin);
    sm_config_set_out_pins(&config, Hardware::SpiMisoPin, 1U);
    sm_config_set_jmp_pin(&config, Hardware::SpiCsPin);
    sm_config_set_out_shift(&config, false, false, 32U);
    sm_config_set_in_shift(&config, true, false, 32U);
    sm_config_set_clkdiv(&config, 1.0F);

    pio_sm_init(pio_, stateMachine_, programOffset_, &config);
    pio_sm_clear_fifos(pio_, stateMachine_);
    pio_sm_set_enabled(pio_, stateMachine_, true);
}

void TransportSPI::service(Diagnostics& diagnostics) noexcept {
    for (std::size_t wordCount = 0U; wordCount < 16U; ++wordCount) {
        if (pio_sm_is_rx_fifo_empty(pio_, stateMachine_)) {
            break;
        }
        processRxWord(pio_sm_get(pio_, stateMachine_), diagnostics);
    }

    serviceTxFifo();
}

bool TransportSPI::enqueueTxPacket(const Protocol::Packet& packet,
                                   Diagnostics& diagnostics) noexcept {
    if (!txQueue_.push(packet)) {
        Diagnostics::increment(diagnostics.txQueueOverflow);
        return false;
    }

    setIrq(true);
    serviceTxFifo();
    return true;
}

bool TransportSPI::popRxPacket(Protocol::Packet& packet) noexcept {
    if (!rxReady_) {
        return false;
    }

    packet = rxPacket_;
    rxReady_ = false;
    return true;
}

void TransportSPI::clearTxQueue() noexcept {
    txQueue_.clear();
    currentTxPacket_ = Protocol::Packet{};
    txFeedIndex_ = 0U;
    txPacketLoaded_ = false;
    txPacketStaged_ = false;

    pio_sm_set_enabled(pio_, stateMachine_, false);
    pio_sm_clear_fifos(pio_, stateMachine_);
    pio_sm_restart(pio_, stateMachine_);
    pio_sm_set_enabled(pio_, stateMachine_, true);

    rxAssembly_ = Protocol::Packet{};
    rxIndex_ = 0U;
    rxFrameState_ = RxFrameState::Collecting;
    rxReady_ = false;
    integrityLost_ = false;
    setIrq(false);
}

void TransportSPI::processRxWord(std::uint32_t word,
                                 Diagnostics& diagnostics) noexcept {
    if (word == FrameEndMarker) {
        completeRxFrame(diagnostics);
        retireTransmittedPacket(diagnostics);
        return;
    }

    const Byte byte = static_cast<Byte>((word >> 24U) & 0xFFU);
    if (rxFrameState_ == RxFrameState::Overflow) {
        return;
    }

    if (rxIndex_ < PacketLength) {
        rxAssembly_[rxIndex_] = byte;
        ++rxIndex_;
    } else {
        rxFrameState_ = RxFrameState::Overflow;
    }
}

void TransportSPI::completeRxFrame(Diagnostics& diagnostics) noexcept {
    if (rxFrameState_ == RxFrameState::Overflow) {
        Diagnostics::increment(diagnostics.longFrame);
    } else if (rxIndex_ != PacketLength) {
        Diagnostics::increment(diagnostics.shortFrame);
    } else if (rxReady_) {
        Diagnostics::increment(diagnostics.rxOverwrite);
    } else {
        rxPacket_ = rxAssembly_;
        rxReady_ = true;
    }

    rxAssembly_ = Protocol::Packet{};
    rxIndex_ = 0U;
    rxFrameState_ = RxFrameState::Collecting;
}

void TransportSPI::serviceTxFifo() noexcept {
    if (!txPacketLoaded_ && !txQueue_.empty()) {
        if (txQueue_.pop(currentTxPacket_)) {
            txPacketLoaded_ = true;
            txFeedIndex_ = 0U;
        }
    }

    if (!txPacketLoaded_ || txPacketStaged_) {
        return;
    }

    while ((txFeedIndex_ < PacketLength) &&
           !pio_sm_is_tx_fifo_full(pio_, stateMachine_)) {
        putTxByte(currentTxPacket_[txFeedIndex_]);
        ++txFeedIndex_;
    }

    if (txFeedIndex_ >= PacketLength) {
        txPacketStaged_ = true;
    }
}

void TransportSPI::retireTransmittedPacket(
    Diagnostics& diagnostics) noexcept {
    if (txPacketLoaded_ && !txPacketStaged_) {
        Diagnostics::increment(diagnostics.txUnderrun);
        txQueue_.clear();
        currentTxPacket_ = Protocol::Packet{};
        txFeedIndex_ = 0U;
        txPacketLoaded_ = false;
        txPacketStaged_ = false;
        integrityLost_ = true;
        setIrq(false);
        return;
    }

    if (txPacketLoaded_ && txPacketStaged_) {
        currentTxPacket_ = Protocol::Packet{};
        txFeedIndex_ = 0U;
        txPacketLoaded_ = false;
        txPacketStaged_ = false;
    }

    serviceTxFifo();
    setIrq(!txIdle());
}

void TransportSPI::setIrq(bool asserted) noexcept {
    const bool physicalHigh = Hardware::SpiIrqActiveHigh ? asserted : !asserted;
    digitalWrite(Hardware::SpiIrqPin, physicalHigh ? HIGH : LOW);
}

void TransportSPI::putTxByte(Byte byte) noexcept {
    pio_sm_put(pio_, stateMachine_, static_cast<std::uint32_t>(byte) << 24U);
}

}  // namespace Mcm
