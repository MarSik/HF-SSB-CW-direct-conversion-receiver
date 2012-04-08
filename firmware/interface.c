#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include "interface.h"
#include "dac.h"
#include "leds.h"

static uint8_t rotary_old;

#define ROTARY_LOOKUP_PREV 0b0100000110000010
#define ROTARY_LOOKUP_NEXT 0b0010100000010100

inline void debounce(void)
{
}

void interface_init(void)
{
    /* setup rotary button + A and B sensors */
    ROTARY_DDR &= ~(_BV(ROTARY_A) | _BV(ROTARY_B) | _BV(ROTARY_BUTTON));
    PORTB |= _BV(ROTARY_A) | _BV(ROTARY_B) | _BV(ROTARY_BUTTON);

    /* setup buttons */
    BUTTON_DDR &= ~(_BV(BUTTON_1) | _BV(BUTTON_2) | _BV(BUTTON_3) | _BV(BUTTON_4)); //inputs
    BUTTON_PORT |= _BV(BUTTON_1) | _BV(BUTTON_2) | _BV(BUTTON_3) | _BV(BUTTON_4); //pull ups

    // save inital rotary state
    rotary_old = ((ROTARY_PIN >> ROTARY_SHIFT) & 0b11) << 2;

    /* rotary encoder pins are change interrupt 16, 17, 18 */
    /* button interrupt pins are 19, 20, 21, 22 */
    PCICR |= _BV(PCIE2);
    PCMSK2 |= _BV(PCINT16) | _BV(PCINT17) | _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT21) | _BV(PCINT22);
}

/* pin change int for key */
ISR(PCINT2_vect){
    if (!state.cw_filter && (BUTTON_PIN & _BV(BUTTON_1)) == 0) {
        set_cw();
        state.redraw = 1;
    }
    if (state.cw_filter && (BUTTON_PIN & _BV(BUTTON_2)) == 0) {
        set_ssb();
        state.redraw = 1;
    }

    /* get rotary vector[oldB oldA B A]*/
    uint8_t r = ((ROTARY_PIN >> ROTARY_SHIFT) & 0b11) | rotary_old;

    if (ROTARY_LOOKUP_NEXT & _BV(r)) {
        // increase freq
    }

    else if (ROTARY_LOOKUP_PREV & _BV(r)) {
        // decrease freq
    }

    /* debounce
    if(db) debounce();
    */

    /* save old rotary */
    rotary_old = ((ROTARY_PIN >> ROTARY_SHIFT) & 0b11) << 2;
}

