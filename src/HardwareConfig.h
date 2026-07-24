#pragma once
#include <Arduino.h>

/**
 * @file HardwareConfig.h
 * @brief KiCad-verified external host-interface constants for the MCM board.
 *
 * This file is the firmware-side source of truth for signals that leave the
 * Modular Control Module and connect to a separate master/controller MCU.
 *
 * Hardware facts captured here:
 *
 * - The released board exposes an SPI host interface only.
 * - There are no external host SDA/SCL nets in the current KiCad design.
 * - All signals are 3.3 V logic.
 * - SPI chip select is active low.
 * - SLAVE_IRQ is active high and is intended to be level-based.
 *
 * @warning The RP2040's QSPI flash pins are a separate internal bus. Never
 *          reuse or reinterpret those nets as the external host SPI port.
 *
 * @note Encoder and button GPIO assignments are documented in
 *       docs/HARDWARE.md but are not yet centralized in this header on main.
 */
namespace McmHardware {

/** MCM-to-master level interrupt: response data is pending. */
static constexpr uint8_t SPI_IRQ_PIN = 11;

/** Master-to-MCM SPI clock. Mode 0 requires this signal to idle low. */
static constexpr uint8_t SPI_SCK_PIN = 12;

/** Master-to-MCM active-low chip select. One assertion frames one packet. */
static constexpr uint8_t SPI_CS_PIN = 13;

/** MCM-to-master SPI data output. */
static constexpr uint8_t SPI_MISO_PIN = 14;

/** Master-to-MCM SPI data input. */
static constexpr uint8_t SPI_MOSI_PIN = 15;

/** Electrical polarity of SPI_IRQ_PIN. */
static constexpr bool SPI_IRQ_ACTIVE_HIGH = true;

/**
 * Size of every wire-protocol packet and every CS-framed transaction.
 *
 * Changing this constant alone does not change the protocol. Any packet-size
 * change requires a protocol-version change, PIO review, master update, test
 * vectors, and an architectural decision record.
 */
static constexpr uint8_t SPI_PACKET_LENGTH = 8;

}  // namespace McmHardware
