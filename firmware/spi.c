#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

void spi_init(void)
{
    SPI_DDR |= _BV(SPI_SCK) | _BV(SPI_MOSI) | _BV(SPI_SS);
    SPI_PORT |= _BV(SPI_SS);

    // Enable SPI, Master, set clock rate fck/128, CLK idle low, sample at H->L
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
    //SPSR |= _BV(SPI2X);
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;

    /* wait for the transfer to complete */
    while(!(SPSR & _BV(SPIF)))
        ;

    return SPDR;
}

void spi_begin(void)
{
    SPI_PORT &= ~_BV(SPI_SS);
    _delay_ms(5);
}

void spi_end(void)
{
    _delay_ms(5);
    SPI_PORT |= _BV(SPI_SS);
}
