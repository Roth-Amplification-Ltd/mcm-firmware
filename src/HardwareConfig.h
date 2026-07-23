#pragma once
#include <Arduino.h>

/*
 * Pin map verified against Modular-Control-Module.kicad_sch/.kicad_pcb.
 * The external host interface is SPI-only; no SDA/SCL host nets exist.
 * All external logic is 3.3 V.
 */
namespace McmHardware {
static constexpr uint8_t SPI_IRQ_PIN  = 11;
static constexpr uint8_t SPI_SCK_PIN  = 12;
static constexpr uint8_t SPI_CS_PIN   = 13;
static constexpr uint8_t SPI_MISO_PIN = 14;
static constexpr uint8_t SPI_MOSI_PIN = 15;
static constexpr bool SPI_IRQ_ACTIVE_HIGH = true;
static constexpr uint8_t SPI_PACKET_LENGTH = 8;
}  // namespace McmHardware
