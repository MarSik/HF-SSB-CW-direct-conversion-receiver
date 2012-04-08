#ifndef __MS_DDS_ir_h__
#define __MS_DDS_ir_h__

/* IR port settings */
#define IRPORT PORTB
#define IRPIN  PINB
#define IRDDR  DDRB
#define IR  2

/* IR port Pin Change INT settings */
/* TODO INT2 set to change mode */
#define IRPCINTREG PCMSK
#define IRPCINTFLAG PCINT3
#define IRPCINTVECT PCINT0_vect

/* IR timer counter reg and INT routine settings */
#define IRTIMER TCNT0
#define IRTIMEROVFINT TIMER0_OVF_vect

/* INT bit to set when IR received valid data */
#define IRDONEINTREG
#define IRDONEINTFLAG

/* INT bit to clear when PCINT routine is called */
#define IROVFREG TIFR
#define IROVFFLAG TOV0

/* IR remote id */
#define IRREMOTEID 0x03

/* setup 8bit IR timer to overflow in 16.384ms */
void inline ir_timer_setup(void)
{
    /* setup timer0 including it's interrupt */
    TCCR0A = 0x00;
    TCCR0B = _BV(CS01) | _BV(CS00); // normal mode 0 - 0xff, prescale by 64
    TIMSK |= _BV(TOIE0); // enable overflow interrupt
}

/* IR buttons */
typedef enum{A_ERROR = 0xfe, A_NONE = 0xff,
             A_PWR = 0x00,
             A_B1  = 0x05,
             A_B2  = 0x06
} action_t;

/* get received IR data */
action_t getIR(void);

/* setup IR */
void ir_init(void);

#endif
