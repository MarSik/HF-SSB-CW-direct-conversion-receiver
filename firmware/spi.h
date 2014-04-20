#ifndef __MT_spi_20120320__
#define __MT_spi_20120320__

#include <avr/io.h>

uint8_t spi_transfer(uint8_t data);
void spi_init(void);
void spi_begin(void);
void spi_end(void);

#endif /* __MT_spi_20120320__ */
