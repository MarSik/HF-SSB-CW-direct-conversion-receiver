#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "spi.h"
#include "tuner.h"

static uint8_t tuner_ctrl = 0;
volatile static uint8_t banks[3] = {0x00, 0x00, 0x00};

// Cout is in 10x pF (1 == 0.1pF)
const static uint16_t cout_values[8] = {37, 75, 147, 303, 576, 1200, 2319, 4807};

// L is in nH (1 == 1nH, 1000 = 1uH)
const static uint16_t l_values[8] = {64, 144, 256, 484, 1024, 2073, 3953, 7938};
const static uint16_t cin_values[8] = {0, 0, 0, 0, 0, 0, 0, 0};

const char* PICO = "pnum kM";

void tuner_write(void)
{
    spi_begin();
    spi_transfer((tuner_ctrl & BANK3_INVERT) ? banks[BANK3] : ~banks[BANK3]);
    spi_transfer((tuner_ctrl & BANK2_INVERT) ? banks[BANK2] : ~banks[BANK2]);
    spi_transfer((tuner_ctrl & BANK1_INVERT) ? banks[BANK1] : ~banks[BANK1]);
    spi_end();
}

void tuner_up(uint8_t bank)
{
    if (banks[bank] < 0xff) ++banks[bank];
}

void tuner_down(uint8_t bank)
{
    if (banks[bank] > 0) --banks[bank];
}

void tuner_init(uint8_t ctrl)
{
    tuner_ctrl = ctrl;

    // Initialize relay driver capacitors to known state
    // Set all relays to 0
    banks[2] = banks[1] = banks[0] = 0;
    tuner_write();

    for (uint8_t i = 0; i<3*8; i++) {

        _delay_ms(1000);

        // Set 1 for a single relay
        banks[(i & 0x18) >> 3] = 1 << (i & 0x7);
        tuner_write();

        _delay_ms(100);

        // Set all relays back to 0
        banks[2] = banks[1] = banks[0] = 0;
        tuner_write();
    }
}

uint8_t tuner_get_cin(void)
{
    return banks[BANK_CIN];
}

uint8_t tuner_get_cout(void)
{
    return banks[BANK_COUT];
}

uint8_t tuner_get_l(void)
{
    return banks[BANK_L];
}

inline uint16_t compute_bits(uint8_t val, const uint16_t table[])
{
    uint8_t index = 0;
    uint16_t ret = 0;

    while(val) {
        if (val & 0x1) ret += table[index];
        index++;
        val >>= 1;
    }

    return ret;
}

uint16_t tuner_get_real_l(void)
{
    return compute_bits(tuner_get_l(), l_values);
}

uint16_t tuner_get_real_cout(void)
{
    return compute_bits(tuner_get_cout(), cout_values) / 10;
}

uint16_t tuner_get_real_cin(void)
{
    return compute_bits(tuner_get_cin(), cin_values) / 10;
}
