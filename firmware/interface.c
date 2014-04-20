#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include "interface.h"
#include "state.h"
#include "radio.h"
#include "tuner.h"

#define SUBSTEP 4
static volatile uint8_t rotary_old;
static volatile int8_t substep = 0;
volatile intf_mode_t interface_mode = INTF_FREQ;

/* lookup table in a form of bitarray (index is the number of the bit,
   counted from 0 LSB)

   inspired by rotary handling from EMRFD
*/
#define ROTARY_LOOKUP_PREV 0b0100000110000010
#define ROTARY_LOOKUP_NEXT 0b0010100000010100

inline void debounce(void)
{
}

void interface_init(void)
{
    /* setup rotary button + A and B sensors */
    ROTARY_DDR &= ~(_BV(ROTARY_A) | _BV(ROTARY_B) | _BV(ROTARY_BUTTON));
    ROTARY_PORT |= _BV(ROTARY_A) | _BV(ROTARY_B) | _BV(ROTARY_BUTTON);

    /* setup buttons */
    BUTTON_DDR &= ~(BUTTON_MASK); //inputs
    BUTTON_PORT |= BUTTON_MASK; //pull ups

    /* setup LEDs */
    LED_DDR |= _BV(LEDA) | _BV(LEDB);
    LED_PORT |= _BV(LEDA) | _BV(LEDB);

#ifdef KEY_ENABLE
    /* morse key interface */
    KEY_DDR &= ~(_BV(KEY_A) | _BV(KEY_B));
    KEY_PORT |= _BV(KEY_A) | _BV(KEY_B);
#endif

    // save inital rotary state
    rotary_old = ((ROTARY_PIN >> ROTARY_SHIFT) & 0b11) << 2;

    /* button interrupt pins are 19, 20, 21, 22 */
    PCICR |= _BV(PCIE2);
    PCMSK2 |= _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT21) | _BV(PCINT22);

    /* serial input 0 PCINT24, PCINT25, also used for morse key */
    /* rotary encoder pins are change interrupt 29, 30, 31 */
    PCICR |= _BV(PCIE3);
    PCMSK3 |= _BV(PCINT24) | _BV(PCINT25) | _BV(PCINT29) | _BV(PCINT30) | _BV(PCINT31);
}

/* pin change int for rotary enc */
ISR(PCINT3_vect){
    if ((ROTARY_PIN & ROTARY_BUTTON) == 0) {

    }

    /* get rotary vector[oldB oldA B A]*/
    uint8_t r = ((ROTARY_PIN >> ROTARY_SHIFT) & 0b11) | rotary_old;

    if (ROTARY_LOOKUP_NEXT & _BV(r)) {
        substep++;
        if (substep>=SUBSTEP) {
            if (interface_mode == INTF_STEP) step_up();
            else if (interface_mode == INTF_TUNER_C) tuner_up(BANK_COUT);
            else if (interface_mode == INTF_TUNER_L) tuner_up(BANK_L);
            else freq_step(F_DIR_UP); // increase freq
            state |= LCD_REDRAW;
            substep -= SUBSTEP;
        }
    }

    else if (ROTARY_LOOKUP_PREV & _BV(r)) {
        substep--;
        if (substep<=-SUBSTEP) {
            if (interface_mode == INTF_STEP) step_down();
            else if (interface_mode == INTF_TUNER_C) tuner_down(BANK_COUT);
            else if (interface_mode == INTF_TUNER_L) tuner_down(BANK_L);
            else freq_step(F_DIR_DOWN); // decrease freq
            state |= LCD_REDRAW;
            substep += SUBSTEP;
        }
    }

    /* debounce
    if(db) debounce();
    */

    /* save old rotary */
    rotary_old = (r & 0b11) << 2;
}

/* pin change int for keys */
ISR(PCINT2_vect){
    if ((BUTTON_PIN & _BV(BUTTON_1)) == 0) {
        if (error) {
            radio_set_error(NULL, 0);
            state |= LCD_REDRAW;
            return;
        }

        tuner_write();
        state |= LCD_REDRAW;
    }

    if ((BUTTON_PIN & _BV(BUTTON_2)) == 0) {
        state |= LCD_REDRAW;
        interface_mode_set(INTF_TUNER_C);
        led_on(LEDB);
    }

    if ((BUTTON_PIN & _BV(BUTTON_3)) == 0) {
        state |= LCD_REDRAW;
        interface_mode_set(INTF_TUNER_L);
        led_on(LEDB);
    }

    if ((BUTTON_PIN & _BV(BUTTON_4)) == 0) {
        state |= LCD_REDRAW;
        interface_mode_set(INTF_STEP);
        led_on(LEDB);
    }

    // no button pressed
    if ((BUTTON_PIN & BUTTON_MASK) == BUTTON_MASK) {
        state |= LCD_REDRAW;
        interface_mode_set(INTF_FREQ);
        led_off(LEDB);
    }
}
