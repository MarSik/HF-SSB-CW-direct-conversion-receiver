#include "spi.h"

void spi_init(void)
{
    SPI_DDR |= _BV(SPI_SCK) | _BV(SPI_MOSI) | _BV(SPI_SS);
    SPI_PORT |= _BV(SPI_SS);

    // Enable SPI, Master, set clock rate fck/4, CLK idle high
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA);
    // SPSR |= _BV(SPI2X);
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;

    /* wait for the transfer to complete */
    while(!(SPSR & _BV(SPIF)))
        ;

    return SPDR;
}
