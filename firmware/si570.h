#ifndef _SI570_H_
#define _SI570_H_

/* The algorithms and data structures used here are only
 * ment for CMOS version of the Si570 chip and for
 * using in QSD/QSE transceiver operating up to 30Mhz
 */

#include "freq.h"

#define SI570_ADDR 0x55
#define SI570_REGISTER 7

// frequencies of the used Si570 in uint32_t used as fixed point using 21 bit for fractional
#define SI570_OUT 0x1c28f5c //output freq divided by 4 (52.320 / 4 Mhz) (11bit.21bit)
#define SI570_XTAL 0xe495c75 //

#define Fdco_min ((freq_t)4856 << (21 - 2)) // target PLL freq 4900Mhz / 4
#define Fdco_max ((freq_t)5670 << (21 - 2)) // maximum PLL freq / 4

#ifdef TEST
void si570_print(void);
#endif

void si570_init(void);
void si570_load(void);
void si570_store(uint8_t freezedco);
uint8_t si570_step_f(int32_t f);
uint8_t si570_set_f(freq_t f);


#endif /* _SI570_H_ */
