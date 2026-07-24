#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mcm_spi_snapshot_protocol.h"

static void expect_packet(const mcm_spi_packet_t *packet, const uint8_t expected[8])
{
    assert(memcmp(packet->byte, expected, 8u) == 0);
    assert(mcm_spi_packet_is_valid(packet));
}

int main(void)
{
    mcm_spi_packet_t packet;
    uint8_t encoded[3];

    static const uint8_t get_snapshot[8] =
        {0xA5, 0x01, 0x81, 0x00, 0x00, 0x00, 0x00, 0x5D};
    static const uint8_t negative_three[8] =
        {0xA5, 0x01, 0x01, 0x02, 0xFD, 0xFF, 0xFF, 0x3F};
    static const uint8_t button_state[8] =
        {0xA5, 0x01, 0x02, 0x00, 0x2A, 0x08, 0x06, 0x12};

    mcm_spi_make_get_snapshot(&packet);
    expect_packet(&packet, get_snapshot);

    mcm_spi_make_param_state(&packet, MCM_CONTROL_EN3, -3);
    expect_packet(&packet, negative_three);
    assert(mcm_spi_param_state_value(&packet) == -3);

    mcm_spi_make_button_state(&packet, 0x2A, 0x08);
    expect_packet(&packet, button_state);
    assert(mcm_spi_button_is_pressed(packet.byte[4], MCM_CONTROL_EN2));
    assert(!mcm_spi_button_is_pressed(packet.byte[4], MCM_CONTROL_EN3));

    mcm_spi_encode_i24(-8388608, encoded);
    assert(mcm_spi_decode_i24(encoded) == -8388608);
    mcm_spi_encode_i24(8388607, encoded);
    assert(mcm_spi_decode_i24(encoded) == 8388607);

    puts("MCM SPI snapshot protocol self-test passed");
    return 0;
}
