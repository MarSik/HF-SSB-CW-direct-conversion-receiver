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
#define IRDONEINTMASK IR_DATA

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
              A_B2  = 0x06
} action_t;

/* get received IR data */
action_t ir_get(void);

/* setup IR */
void ir_init(void);

#endif
