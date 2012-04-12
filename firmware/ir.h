#ifndef __MS_DDS_ir_h__
#define __MS_DDS_ir_h__

#include <stdint.h>
#include "state.h"

/* IR port settings */
#define IRPORT PORTB
#define IRPIN  PINB
#define IRDDR  DDRB
#define IR  2

/* IR port Pin Change INT settings */
/* TODO INT2 set to change mode */
#define IRPCINTREG EIMSK
#define IRPCINTFLAG INT2
#define IRPCINTVECT INT2_vect

void inline ir_pcint_setup(void)
{
    // any edge generates interrupt
    EICRA &= ~_BV(ISC21);
    EICRA |= _BV(ISC20);

    // enable INT2
    IRPCINTREG |= _BV(IRPCINTFLAG);
}

/* IR timer counter reg and INT routine settings */
#define IRTIMER TCNT0
#define IRTIMEROVFINT TIMER0_COMPA_vect

/* INT bit to set when IR received valid data */
#define IRDONEINTREG state
#define IRDONEINTMASK IR_DATA_READY

/* INT bit to clear when PCINT routine is called */
#define IROVFREG TIFR0
#define IROVFFLAG OCF0A

/* IR remote id */
#define IRREMOTEID 0x03

/* setup 8bit IR timer to overflow in 16.384ms */
void inline ir_timer_setup(void)
{
    /* setup timer0 including it's interrupt */
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS02) | _BV(CS00); // CTC mode 0 - OCR0A, prescale by 1024
    OCR0A = 127;
    TIMSK0 |= _BV(OCIE0A); // enable overflow interrupt
}

/* IR buttons */
typedef enum{ A_ERROR = 0xfe, A_NONE = 0xff,
              A_PWR = 0x00,
              A_B1  = 0x05,
              A_B2  = 0x06,
              A_B3  = 0x07,
              A_B4  = 0x09,
              A_B5  = 0x0a,
              A_B6  = 0x0b,
              A_B7  = 0x0d,
              A_B8  = 0x0e,
              A_B9  = 0x0f,
              A_B0  = 0x12,
              A_TV  = 0x56,
              A_FM  = 0x57,
              A_DVD = 0x58,
              A_RED = 0x4b,
              A_GREEN = 0x4c,
              A_YELLOW = 0x4d,
              A_BLUE = 0x4e,
              A_CC = 0x1f,
              A_ZZ = 0x40,
              A_MUTE = 0x14,
              A_BOSS = 0x49,
              A_C = 0x50,
              A_FULL = 0x3,
              A_MENU = 0x4f,
              A_GRID = 0x51,
              A_CHUP = 0xc,
              A_CHDOWN = 0x10,
              A_VUP = 0x4,
              A_VDOWN = 0x8,
              A_FIRST = 0x42,
              A_PLAY = 0x43,
              A_LAST = 0x44,
              A_REW = 0x54,
              A_STOP = 0x46,
              A_FWD = 0x55,
              A_SUBS = 0x52,
              A_LANG = 0x53,
              A_VIDEO = 0x1e,
              A_AUDIO = 0x1b,
              A_DISPLAY = 0x16,
              A_M = 0x48,
              A_DOT = 0x41,
              A_BACK = 0x11,
              A_ENTER = 0x13,
              A_PIP = 0x4a,
              A_RECSTOP = 0x46,
              A_REC = 0x47,
              A_TIMESHIFT = 0x45
} action_t;

/* get received IR data */
action_t ir_get(void);

/* setup IR */
void ir_init(void);

#endif
