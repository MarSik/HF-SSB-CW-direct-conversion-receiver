#ifndef __DDS_MS_state_h__
#define __DDS_MS_state_h__

#include <avr/io.h>

#define state GPIOR0

#define ST_CW _BV(0)
#define IR_DATA_READY _BV(4)
#define LCD_REDRAW _BV(5)
#define F_SMALL_CHANGE _BV(6)
#define F_CHANGED _BV(7)

#endif
