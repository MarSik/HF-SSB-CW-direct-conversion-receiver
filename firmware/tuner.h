#ifndef __DDS_tuner_h__
#define __DDS_tuner_h__

#include <stdint.h>
#include "spi.h"

void tuner_write();
void tuner_init(uint8_t ctrl);

void tuner_up(uint8_t bank);
void tuner_down(uint8_t bank);

#define BANK1_INVERT 0x1
#define BANK2_INVERT 0x2
#define BANK3_INVERT 0x4

#define BANK_CIN 2
#define BANK_L 1
#define BANK_COUT 0

#endif
