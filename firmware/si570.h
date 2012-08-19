#ifndef _SI570_H_
#define _SI570_H_

#define SI570_ADDR 0x55

// frequencies of the used Si570 in uint32_t used as fixed point using 21 bit for fractional
#define SI570_OUT 0x1c28f5c //output freq divided by 4 (52.320 / 4 Mhz) (11bit.21bit)
#define SI570_XTAL 0xe495c75 //

#ifdef TEST
void si570_print(void);
#endif

void si570_init(void);
void si570_load(void);
void si570_store(uint8_t freezedco);
uint8_t si570_step_f(int16_t f);
uint8_t si570_set_f(uint32_t f);


#endif /* _SI570_H_ */
