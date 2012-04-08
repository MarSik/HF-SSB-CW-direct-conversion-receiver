#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "ir.h"

/*
  Set the timer so it overflows in 16.384ms (space between messages is 39ms)
  that means we are capturing time with 0.064ms resolution (15.625 per ms)

  Protocol of Leadtek WinFast Y040052 remote

  Signal Length CNT Meaning

  0      >10ms  OVF timer overflow while at 0 means error (sensor disconnected?)
  0      9 ms   229 First in the message, announces new message
  0      0.64ms  16 Ends a bit, get the length of preceeding 1

  1      >10ms  OVF timer overflow while at 1 means end of the message
  1      4.48ms 114 If it is the first bit, then it is start bit 
  1      2.1ms   53 If it is the first bit, then it is also the last and denotes repetition of previous message
  1      1.6ms   40 1
  1      0.48ms  12 0
*/


#define START_BIT_L 50 //3.2ms @ 1Mhz, 64 prescale
#define REPEAT_L 31 //1.984ms
#define ONE_BIT_L 16 //1.024ms
#define PREAMBLE_L 93 //5.952ms


action_t inline processIR(volatile uint8_t msg[4])
{
    /* check the inverted bytes for errors */
    if (((msg[0] ^ msg[1]) != 0xff) || 
        ((msg[2] ^ msg[3]) != 0xff)) return A_ERROR;

    /* check remote id for match */
    if (msg[0] != IRREMOTEID) return A_NONE;

    /* reset state to nothing */
    state = D_NOTHING;

    /* return action/button */
    return (action_t)(msg[2]);
}

action_t getIR(void)
{
    return processIR(data);
}

static volatile uint8_t bit_c = 0; // bit no. we are about to receive (255 denotes repeated command, 254 not ready state)
static volatile uint8_t data[4];
static volatile enum{D_NOTHING = 0,
              D_PREAMBLE,
              D_RECEIVING_NEEDACK,
              D_RECEIVING,
              D_ALLREAD,
              D_DATA,
              D_REPEAT} state = D_NOTHING;

void ir_init(void)
{
    /* set pin directions */
    IRDDR &= ~_BV(IR); // inputs
    IRPORT &= ~_BV(IR); // no pull-up

    /* enable change interrupts */
    GIMSK |= _BV(PCIE);
    IRPCINTREG |= _BV(IRPCINTFLAG);

    ir_timer_setup();
}

/* Pin change interrupt - reading IR data */
ISR(IRPCINTVECT) {
    static uint8_t oldbit = _BV(IR); // to ensure change has happened

    // not an IR interrupt
    uint8_t bit = IRPIN & _BV(IR); // current IR pin state
    if (oldbit == bit) return;

    // reset IR timer
    uint8_t timer = IRTIMER;
    IRTIMER = 0;
    IROVFREG |= _BV(IROVFFLAG); // reset overflow interrupt flag

    // record the change
    oldbit = bit;

    if (state >= D_ALLREAD) return; // we only care about first 32 bits and only if we are ready

    // check data
    if (!bit){ // we are currently low so high bit duration was measured 
        if (timer > START_BIT_L && state == D_PREAMBLE) { // start bit
            bit_c = 0;
            state = D_RECEIVING_NEEDACK;
        }
        else if (timer > REPEAT_L && state == D_PREAMBLE) bit_c = 255; // repetition flag
        else if (timer > ONE_BIT_L && state == D_RECEIVING) {
            data[bit_c >> 3] = (data[bit_c >> 3] >> 1) + 0x80; // shift a log. 1, bit_c >> 3 is the byte number
        }
        else if(state == D_RECEIVING) {
            data[bit_c >> 3] >>= 1; // shift a log. 0
        }
    }
    else{ // low bit timed
        if (timer > PREAMBLE_L && state == D_NOTHING) state = D_PREAMBLE; // preamble low bit (9ms)
        else if (state == D_RECEIVING) {
            if (bit_c == 255) state = D_ALLREAD; // repetition has only one data bit
            else {
                bit_c++; // bit marker
                if (bit_c == 32) state = D_ALLREAD; // all 32 bits received
            }
        }
        else if (state == D_RECEIVING_NEEDACK) state = D_RECEIVING;
    }
}

ISR(IRTIMEROVFINT) {
    if(state == D_ALLREAD){
        if(bit_c == 255) state = D_REPEAT; // special flag set, this requests repetition of previous command
        else state = D_DATA; // data received

        // set int flag
        IRDONEINTREG |= _BV(IRDONEINTFLAG);
    }
    else state = D_NOTHING; // timeout, reset the state
}
