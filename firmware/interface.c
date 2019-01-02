#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <string.h>
#include "interface.h"
#include "state.h"
#include "radio.h"
#include "tuner.h"
#include "lang.h"
#include "lcd.h"
#include "freq.h"

#define SUBSTEP 4
static volatile uint8_t rotary_old;
static volatile int8_t substep = 0;
volatile intf_mode_t interface_mode = INTF_FREQ;

uint8_t buffer[9];

/* lookup table in a form of bitarray (index is the number of the bit,
   counted from 0 LSB)

   inspired by rotary handling from EMRFD
*/
#define ROTARY_LOOKUP_PREV 0b0100000110000010
#define ROTARY_LOOKUP_NEXT 0b0010100000010100

void renderer_default(void);
void renderer_step(void);
void renderer_set_l(void);
void renderer_set_c(void);
void renderer_set_mode(void);

typedef void (*renderer_func)(void);

static renderer_func renderer[] = {
    [INTF_FREQ] = renderer_default,
    [INTF_STEP] = renderer_step,
    [INTF_TUNER_L] = renderer_set_l,
    [INTF_TUNER_C] = renderer_set_c,
    [INTF_TUNER_MODE] = renderer_set_mode
};

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
            if (interface_mode == INTF_STEP) step_down();
            else if (interface_mode == INTF_TUNER_C) tuner_up(BANK_COUT);
            else if (interface_mode == INTF_TUNER_L) tuner_up(BANK_L);
            else if (interface_mode == INTF_TUNER_MODE) tuner_next_mode();
            else freq_step(F_DIR_UP); // increase freq
            state |= LCD_REDRAW;
            substep -= SUBSTEP;
        }
    }

    else if (ROTARY_LOOKUP_PREV & _BV(r)) {
        substep--;
        if (substep<=-SUBSTEP) {
            if (interface_mode == INTF_STEP) step_up();
            else if (interface_mode == INTF_TUNER_C) tuner_down(BANK_COUT);
            else if (interface_mode == INTF_TUNER_L) tuner_down(BANK_L);
            else if (interface_mode == INTF_TUNER_MODE) tuner_prev_mode();
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
        tuner_save();
        state |= LCD_REDRAW;
    }

    if ((BUTTON_PIN & _BV(BUTTON_2)) == 0
           && (BUTTON_PIN & _BV(BUTTON_3)) == 0) {
        state |= LCD_REDRAW | LCD_CLEAR;
        interface_mode_set(INTF_TUNER_MODE);
        led_on(LEDB);
    }

    if ((BUTTON_PIN & _BV(BUTTON_2)) == 0) {
        state |= LCD_REDRAW | LCD_CLEAR;
        interface_mode_set(INTF_TUNER_C);
        led_on(LEDB);
    }

    if ((BUTTON_PIN & _BV(BUTTON_3)) == 0) {
        state |= LCD_REDRAW | LCD_CLEAR;
        interface_mode_set(INTF_TUNER_L);
        led_on(LEDB);
    }

    if ((BUTTON_PIN & _BV(BUTTON_4)) == 0) {
        state |= LCD_REDRAW | LCD_CLEAR;
        interface_mode_set(INTF_STEP);
        led_on(LEDB);
    }

    // no button pressed
    if ((BUTTON_PIN & BUTTON_MASK) == BUTTON_MASK) {
        state |= LCD_REDRAW | LCD_CLEAR;
        interface_mode_set(INTF_FREQ);
        led_off(LEDB);
    }
}

void interface_render(void)
{
    renderer[interface_mode]();
}

void renderer_freq(void)
{
    lcd_line(0);
    lcd_mode(LCD_DATA);

    // print Mhz
    if (f > 0) {
        f2str(f, buffer, 10);
        if (strlen(buffer) < 10) lcd_put(' ');
        lcd_write(buffer);

        lcd_put(' ');

        if (f_MHZ(f_step)) {
            utoa(f_MHZ(f_step), buffer, 10);
            for(uint8_t i=3; i>strlen(buffer); i--) lcd_put(' ');
            lcd_write(buffer);
            lcd_put('M');
        }
        else if (f_KHZ(f_step)) {
            utoa(f_KHZ(f_step), buffer, 10);
            for(uint8_t i=3; i>strlen(buffer); i--) lcd_put(' ');
            lcd_write(buffer);
            lcd_put('k');
        }
        else {
            utoa(f_HZ(f_step), buffer, 10);
            for(uint8_t i=4; i>strlen(buffer); i--) lcd_put(' ');
            lcd_write(buffer);
        }

        if (state & _BV(TUNER_MEM)) lcd_put(0);
        else lcd_put(' ');
    }
    else lcd_pgm_write(s_initializing);
}

void renderer_step(void)
{
    const uint8_t *extra = NULL;

    renderer_freq();

    lcd_line(1);
    lcd_mode(LCD_DATA);


    uint8_t logstep = 15;

    while (logstep > 0) {
        lcd_put(' ');
        logstep--;
    }

    lcd_line(1);
    lcd_mode(LCD_DATA);

    freq_t step = f_full(f_step);
    while (step > 1) {
        logstep++;
        step /= 10;
    }

    logstep = 9 - (logstep + logstep / 3);
    while (logstep > 0) {
        lcd_put(' ');
        logstep--;
    }
    lcd_put(1);
}

void renderer_default(void)
{
    const uint8_t *extra = NULL;

    renderer_freq();

    lcd_line(1);
    lcd_mode(LCD_DATA);

    if (error) {
        lcd_pgm_write(error);
        utoa(error_id, buffer, 16);
        lcd_write(buffer);
    }
    else if (f>0) {
        uint8_t fl = bandplan(f_sf(f), &extra);

        uint16_t v;
        uint8_t r;

        /*
        v = tuner_get_real_cin();
        r = 0;
        while(v > 1000) {
            v /= 1000;
            r++;
        }

        utoa(v, buffer, 10);
        for(uint8_t i=4; i>strlen(buffer); i--) lcd_put(' ');
        lcd_write(buffer);
        lcd_put(PICO[r]);
        lcd_put('F');
        */

        v = tuner_get_real_l();
        r = 0;
        while(v > 1000) {
            v /= 1000;
            r++;
        }

        utoa(v, buffer, 10);
        for(uint8_t i=4; i>strlen(buffer); i--) lcd_put(' ');
        lcd_write(buffer);
        lcd_put(PICO[r+1]);
        lcd_put('H');

        v = tuner_get_real_cout();
        r = 0;
        while(v > 1000) {
            v /= 1000;
            r++;
        }

        utoa(v, buffer, 10);
        for(uint8_t i=4; i>strlen(buffer); i--) lcd_put(' ');
        lcd_write(buffer);
        lcd_put(PICO[r]);
        lcd_put('F');
    }
}

void renderer_set_c(void)
{
    renderer_freq();

    lcd_line(1);
    lcd_mode(LCD_DATA);

    uint8_t r;
    uint16_t v;

    r = tuner_get_cout();

    utoa(r, buffer, 10);
    for(uint8_t i=4; i>strlen(buffer); i--) lcd_put(' ');
    lcd_write(buffer);

    v = tuner_get_real_cout();

    utoa(v, buffer, 10);
    for(uint8_t i=7; i>strlen(buffer); i--) lcd_put(' ');
    lcd_write(buffer);
    lcd_put(PICO[0]);
    lcd_put('F');
}

static const char* T_MODE EEMEM = "tuner mode:";
static const char* T_VERTICAL EEMEM = "\x02 vertical";
static const char* T_SYMETRIC EEMEM = "\x03 symetric";

void renderer_set_mode(void)
{
    lcd_line(0);
    lcd_mode(LCD_DATA);

    lcd_eep_write(T_MODE);

    lcd_line(1);
    lcd_mode(LCD_DATA);

    if (tuner_get_mode() == SYMETRIC) {
        lcd_eep_write(T_SYMETRIC);
    } else {
        lcd_eep_write(T_VERTICAL);
    }
}

void renderer_set_l(void)
{
    renderer_freq();

    lcd_line(1);
    lcd_mode(LCD_DATA);

    uint8_t r;
    uint16_t v;

    r = tuner_get_l();
    utoa(r, buffer, 10);
    for(uint8_t i=4; i>strlen(buffer); i--) lcd_put(' ');
    lcd_write(buffer);

    v = tuner_get_real_l();
    utoa(v, buffer, 10);
    for(uint8_t i=7; i>strlen(buffer); i--) lcd_put(' ');
    lcd_write(buffer);
    lcd_put(PICO[1]);
    lcd_put('H');
}
