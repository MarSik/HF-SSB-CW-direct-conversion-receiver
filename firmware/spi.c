#include "spi.h"

void spi_init(void)
{
    SPI_DDR |= _BV(SPI_SCK) | _BV(SPI_MOSI);
    SPI_PORT |= _BV(SPI_SCK); // SCK idle high

    // Enable SPI, Master, set clock rate fck/2
    SPCR = _BV(SPE)|_BV(MSTR);
    SPSR |= _BV(SPI2X);
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;

    /* wait for the transfer to complete */
    while(!(SPSR & (1<<SPIF)))
        ;

    return SPDR;
}
