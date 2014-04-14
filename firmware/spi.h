#ifndef __MT_spi_20120320__
#define __MT_spi_20120320__

#include <avr/io.h>

#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_MISO 6
#define SPI_MOSI 5
#define SPI_SCK 7
#define SPI_SS 4
#define SPI_SS2 3

uint8_t spi_transfer(uint8_t data);
void spi_init(void);
void spi_begin(void);
void spi_end(void);

#endif /* __MT_spi_20120320__ */
