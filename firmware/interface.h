#ifndef __MT_interface__ms_20120326__
#define __MT_interface__ms_20120326__

#include <avr/io.h>

#define ROTARY_PORT PORTC
#define ROTARY_PIN PINC
#define ROTARY_DDR DDRC
#define ROTARY_BUTTON 0
#define ROTARY_A 1
#define ROTARY_B 2
#define ROTARY_SHIFT 1 // how much bits must be shifted left to get AB on the two LSB (mask 0b11)
#define ROTARY_DIR 1 //1 or -1 if need to reverse rotation arises

#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_DDR DDRC
#define BUTTON_1 3
#define BUTTON_2 4
#define BUTTON_3 5
#define BUTTON_4 6

void interface_init(void);

#endif
