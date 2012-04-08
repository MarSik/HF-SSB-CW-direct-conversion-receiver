#ifndef __MT_spi_20120320__
#define __MT_spi_20120320__

#include <avr/io.h>

#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_MISO 6
#define SPI_MOSI 5
#define SPI_SCK 7

extern volatile uint8_t spi_pause_data;
extern volatile uint8_t spi_pause_clk;

uint8_t spi_transfer(uint8_t data);

inline void spi_pause(void)
{
    spi_pause_clk = SPI_PORT & _BV(SPI_SCK);
    spi_pause_data = USIDR;
}

inline void spi_unpause(void)
{
    SPI_PORT &= ~_BV(SPI_SCK);
    SPI_PORT |= spi_pause_clk;
    USIDR = spi_pause_data;
}

void spi_init(void);

#endif /* __MT_spi_20120320__ */
