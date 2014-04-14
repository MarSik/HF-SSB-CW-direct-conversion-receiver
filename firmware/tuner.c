#include <avr/io.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "spi.h"
#include "tuner.h"

static uint8_t tuner_ctrl = 0;
static uint8_t banks[3] = {0x00, 0x00, 0x00};

void tuner_write()
{
    spi_begin();
    spi_transfer((tuner_ctrl & BANK3_INVERT) ? ~banks[2] : banks[2]);
    spi_transfer((tuner_ctrl & BANK2_INVERT) ? ~banks[1] : banks[1]);
    spi_transfer((tuner_ctrl & BANK1_INVERT) ? ~banks[0] : banks[0]);
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
}
