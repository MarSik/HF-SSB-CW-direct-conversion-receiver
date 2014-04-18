#ifndef __DDS_tuner_h__
#define __DDS_tuner_h__

#include <stdint.h>
#include "spi.h"

void tuner_write(void);
void tuner_init(void);

void tuner_up(uint8_t bank);
void tuner_down(uint8_t bank);

#define BANK1_INVERT 0
#define BANK2_INVERT 0
#define BANK3_INVERT 1

#define BANK_CIN 0
#define BANK_L 1
#define BANK_COUT 2

#define BANK1 BANK_COUT
#define BANK2 BANK_CIN
#define BANK3 BANK_L

uint8_t tuner_get_cin(void);
uint8_t tuner_get_cout(void);
uint8_t tuner_get_l(void);

uint16_t tuner_get_real_l(void);
uint16_t tuner_get_real_cout(void);
uint16_t tuner_get_real_cin(void);

extern const char* PICO;

#endif
