# MCM Firmware

Firmware for the Roth Amplification Modular Control Module, an RP2040-based control surface programmed with the Arduino-Pico core.

## Host interface

The current MCM PCB uses **SPI only** for communication with its host/master controller. Earlier architecture notes described optional SPI and I2C transports, but the released KiCad design does not route SDA or SCL and the firmware must not advertise or depend on an I2C host interface.

The MCM operates as an SPI Mode 0 peripheral/slave using 3.3 V logic:

| Signal | RP2040 GPIO | Direction |
|---|---:|---|
| `SLAVE_IRQ` | 11 | MCM to master |
| `SPI_SCK` | 12 | Master to MCM |
| `SPI_CS` | 13 | Master to MCM, active low |
| `SPI_MISO` | 14 | MCM to master |
| `SPI_MOSI` | 15 | Master to MCM |

`SLAVE_IRQ` is active high and remains asserted while response packets are pending. Each chip-select frame carries one fixed eight-byte protocol packet.

The board's QSPI flash bus is internal to the RP2040 subsystem and is unrelated to the external host SPI connector.
