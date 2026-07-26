/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MCM_SPI_SNAPSHOT_PROTOCOL_H
#define MCM_SPI_SNAPSHOT_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Portable byte-oriented helpers for the MCM SPI snapshot protocol.
 *
 * This header deliberately does not define a packed wire struct. Every packet
 * is represented as eight bytes to avoid ABI padding and endian assumptions.
 */

enum {
    MCM_SPI_PACKET_SIZE = 8,
    MCM_SPI_SYNC_BYTE = 0xA5,
    MCM_SPI_PROTOCOL_VERSION = 0x01,
    MCM_CONTROL_COUNT = 6,
    MCM_BUTTON_BANK_0 = 0
};

typedef enum mcm_spi_message_type {
    MCM_MSG_PARAM_STATE      = 0x01,
    MCM_MSG_BUTTON_STATE     = 0x02,
    MCM_MSG_SNAPSHOT_BEGIN   = 0x10,
    MCM_MSG_SNAPSHOT_END     = 0x11,

    MCM_MSG_CMD_NOP          = 0x80,
    MCM_MSG_CMD_GET_SNAPSHOT = 0x81,
    MCM_MSG_CMD_RESET_PARAM  = 0x82,
    MCM_MSG_CMD_RESET_ALL    = 0x83
} mcm_spi_message_type_t;

typedef enum mcm_control_index {
    MCM_CONTROL_EN1 = 0,
    MCM_CONTROL_EN2 = 1,
    MCM_CONTROL_EN3 = 2,
    MCM_CONTROL_EN4 = 3,
    MCM_CONTROL_EN5 = 4,
    MCM_CONTROL_EN6 = 5
} mcm_control_index_t;

typedef enum mcm_snapshot_status_flags {
    MCM_SNAPSHOT_STATUS_EVENT_QUEUE_OVERFLOW = 1u << 0,
    MCM_SNAPSHOT_STATUS_CONTROL_SCAN_OVERRUN = 1u << 1
} mcm_snapshot_status_flags_t;

typedef struct mcm_spi_packet {
    uint8_t byte[MCM_SPI_PACKET_SIZE];
} mcm_spi_packet_t;

typedef struct mcm_control_snapshot {
    uint16_t sequence_id;
    int32_t encoder[MCM_CONTROL_COUNT];
    uint8_t button_pressed_bitmap;
    uint8_t button_long_bitmap;
    uint8_t status_flags;
} mcm_control_snapshot_t;

static inline uint8_t mcm_spi_crc8(const uint8_t *data, size_t length)
{
    uint8_t crc = 0x00u;
    size_t i;

    for (i = 0; i < length; ++i) {
        uint8_t bit;
        crc ^= data[i];
        for (bit = 0; bit < 8u; ++bit) {
            crc = (crc & 0x80u)
                ? (uint8_t)((uint8_t)(crc << 1u) ^ 0x07u)
                : (uint8_t)(crc << 1u);
        }
    }
    return crc;
}

static inline int mcm_spi_packet_is_valid(const mcm_spi_packet_t *packet)
{
    if (packet == NULL) {
        return 0;
    }
    if (packet->byte[0] != MCM_SPI_SYNC_BYTE) {
        return 0;
    }
    if (packet->byte[1] != MCM_SPI_PROTOCOL_VERSION) {
        return 0;
    }
    return packet->byte[7] == mcm_spi_crc8(packet->byte, 7u);
}

static inline void mcm_spi_finalize_packet(mcm_spi_packet_t *packet)
{
    packet->byte[7] = mcm_spi_crc8(packet->byte, 7u);
}

static inline void mcm_spi_init_packet(
    mcm_spi_packet_t *packet,
    uint8_t type,
    uint8_t index,
    uint8_t data0,
    uint8_t data1,
    uint8_t data2)
{
    packet->byte[0] = MCM_SPI_SYNC_BYTE;
    packet->byte[1] = MCM_SPI_PROTOCOL_VERSION;
    packet->byte[2] = type;
    packet->byte[3] = index;
    packet->byte[4] = data0;
    packet->byte[5] = data1;
    packet->byte[6] = data2;
    mcm_spi_finalize_packet(packet);
}

static inline void mcm_spi_make_get_snapshot(mcm_spi_packet_t *packet)
{
    mcm_spi_init_packet(packet, MCM_MSG_CMD_GET_SNAPSHOT, 0u, 0u, 0u, 0u);
}

static inline void mcm_spi_make_snapshot_boundary(
    mcm_spi_packet_t *packet,
    uint8_t type,
    uint16_t sequence_id,
    uint8_t status_flags)
{
    mcm_spi_init_packet(
        packet,
        type,
        MCM_CONTROL_COUNT,
        (uint8_t)(sequence_id & 0xFFu),
        (uint8_t)((sequence_id >> 8u) & 0xFFu),
        status_flags);
}

static inline void mcm_spi_encode_i24(int32_t value, uint8_t output[3])
{
    uint32_t raw = (uint32_t)value & 0x00FFFFFFu;
    output[0] = (uint8_t)(raw & 0xFFu);
    output[1] = (uint8_t)((raw >> 8u) & 0xFFu);
    output[2] = (uint8_t)((raw >> 16u) & 0xFFu);
}

static inline int32_t mcm_spi_decode_i24(const uint8_t input[3])
{
    uint32_t raw = (uint32_t)input[0]
                 | ((uint32_t)input[1] << 8u)
                 | ((uint32_t)input[2] << 16u);

    if ((raw & 0x00800000u) != 0u) {
        raw |= 0xFF000000u;
    }
    return (int32_t)raw;
}

static inline void mcm_spi_make_param_state(
    mcm_spi_packet_t *packet,
    uint8_t control_index,
    int32_t value)
{
    uint8_t encoded[3];
    mcm_spi_encode_i24(value, encoded);
    mcm_spi_init_packet(
        packet,
        MCM_MSG_PARAM_STATE,
        control_index,
        encoded[0],
        encoded[1],
        encoded[2]);
}

static inline int32_t mcm_spi_param_state_value(const mcm_spi_packet_t *packet)
{
    return mcm_spi_decode_i24(&packet->byte[4]);
}

static inline void mcm_spi_make_button_state(
    mcm_spi_packet_t *packet,
    uint8_t pressed_bitmap,
    uint8_t long_bitmap)
{
    mcm_spi_init_packet(
        packet,
        MCM_MSG_BUTTON_STATE,
        MCM_BUTTON_BANK_0,
        (uint8_t)(pressed_bitmap & 0x3Fu),
        (uint8_t)(long_bitmap & 0x3Fu),
        MCM_CONTROL_COUNT);
}

static inline int mcm_spi_button_is_pressed(uint8_t bitmap, uint8_t control_index)
{
    return control_index < MCM_CONTROL_COUNT
        ? (int)((bitmap >> control_index) & 0x01u)
        : 0;
}

static inline uint16_t mcm_spi_snapshot_sequence(const mcm_spi_packet_t *packet)
{
    return (uint16_t)packet->byte[4]
         | (uint16_t)((uint16_t)packet->byte[5] << 8u);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MCM_SPI_SNAPSHOT_PROTOCOL_H */
