#ifndef __MS_DDS_bandplan_h__
#define __MS_DDS_bandplan_h__

#define CW _BV(0)
#define SSB _BV(1)
#define DIGI _BV(2)
#define FM _BV(3)
#define _MODE_RESERVED _BV(4)
#define QRP _BV(5)
#define DX _BV(6)
#define TX _BV(7)

uint8_t bandplan(uint16_t kHz, const uint8_t **desc);

#endif
