#ifndef _SI570_H_
#define _SI570_H_

/* The algorithms and data structures used here are only
 * ment for CMOS version of the Si570 chip and for
 * using in QSD/QSE transceiver operating up to 30Mhz
 */

#include "freq.h"

#define SI570_ADDRESS 0x55
#define SI570_REGISTER 7
#define SI570_MEM_REGISTER 135
#define SI570_DCO_REGISTER 137

// frequencies of the used Si570 in uint32_t used as fixed point using 1 bit for fractional
#define SI570_OUT HZ_f(14, 80, 0) //output freq divided by 4 (52.320 / 4 Mhz)
#define SI570_XTAL FHZ_f(114, 292, 536, 721)  //internal xtal frequency

#define Fdco_min MHZ_f(4856 / 4) // target PLL freq 4856Mhz / 4
#define Fdco_max MHZ_f(5670 / 4) // maximum PLL freq 5670Mhz / 4

#ifdef TEST
void si570_print(void);
#endif

uint8_t si570_init(void);
uint8_t si570_store(uint8_t freezedco);
uint8_t si570_step_f(int32_t f);
int si570_set_f(freq_t f);


#endif /* _SI570_H_ */
