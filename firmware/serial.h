#ifndef MS_TUNER_SERIAL_H_1404
#define MS_TUNER_SERIAL_H_1404

#include <inttypes.h>

void serial_init(void);

void serial0_putch(uint8_t ch);
void serial0_puts(uint8_t *s);

void serial1_putch(uint8_t ch);
void serial1_puts(uint8_t *s);

#endif
