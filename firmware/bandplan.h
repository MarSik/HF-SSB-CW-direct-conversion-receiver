#ifndef __MS_DDS_bandplan_h__
#define __MS_DDS_bandplan_h__

#include <stdint.h>

/* table
   2B - starting kHz, uint16_t
   2B - ending kHz, uint16_t
   1B - flags
   1B - extra str, uint8_t
*/

#define BAND(from, to, flags, extra) (from), (to), ((flags) << 8) + (extra)

extern uint16_t bandplan_table[];

#define CW _BV(0)
#define SSB _BV(1)
#define DIGI _BV(2)
#define FM _BV(3)
#define DX _BV(6)
#define TX _BV(7)

uint8_t bandplan(uint16_t kHz, const uint8_t **desc);

#endif
