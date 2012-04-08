#include "spi.h"

volatile uint8_t spi_pause_data;
volatile uint8_t spi_pause_clk;

void spi_init(void)
{
    SPI_DDR |= _BV(SPI_SCK) | _BV(SPI_MOSI);
    SPI_PORT |= _BV(SPI_SCK); // SCK idle high
    USICR = 0; // TODO SPI mode, soft strobes
}

uint8_t spi_transfer(uint8_t data)
{
    uint8_t clock = USICR | _BV(USITC);
    uint8_t clock_shift = USICR | _BV(USITC) | _BV(USICLK);

    USIDR = data;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    USICR = clock;
    USICR = clock_shift;

    return USIDR;
}
