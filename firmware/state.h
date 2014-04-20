#ifndef __DDS_MS_state_h__
#define __DDS_MS_state_h__

#include <avr/io.h>

#define state GPIOR0

#define _NOP1 _BV(0)
#define IR_DATA_READY _BV(4)
#define LCD_REDRAW _BV(5)
#define LCD_CLEAR _BV(6)
#define F_CHANGED _BV(7)

#endif
