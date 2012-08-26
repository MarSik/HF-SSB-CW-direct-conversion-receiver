#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "state.h"
#include "radio.h"
#include "freq.h"
#include "si570.h"

volatile freq_t f;
volatile freq_t f_staged;
volatile freq_t f_step;

volatile char *error = NULL;
volatile uint8_t error_id;

void radio_set_error(char *eep_string, uint8_t id)
{
    error = eep_string;
    error_id = id;
    state |= LCD_REDRAW;
}

void set_cw(void)
{
    FILTER_PORT |= _BV(FILTER_CW);
    _delay_ms(30);
    FILTER_PORT &= ~_BV(FILTER_CW);
    state |= ST_CW;
}

void set_ssb(void)
{
    FILTER_PORT |= _BV(FILTER_SSB);
    _delay_ms(30);
    PORTD &= ~_BV(FILTER_SSB);
    state &= ~ST_CW;
}


void freq_step(signed char dir)
{
    if (f_staged) return;

    f_staged = f + OFFSET_DIR * dir * f_step;

    if ((f_staged < F_MIN) || (f_staged>(0xffffffff - F_MAX))) f_staged = F_MIN;
    else if (f_staged>F_MAX) f_staged = F_MAX;

    state |= F_SMALL_CHANGE;
}

void step_up()
{
    f_step *= 10;
    if (f_step > STEP_MAX) f_step = STEP_MAX;
}

void step_down()
{
    f_step /= 10;
    if (f_step < STEP_MIN) f_step = STEP_MIN;
}


void radio_init(void)
{
    FILTER_DDR |= _BV(FILTER_CW) | _BV(FILTER_SSB);
    FILTER_PORT &= ~(_BV(FILTER_CW) | _BV(FILTER_SSB));

    set_cw();

    f = 0;
    f_staged = SI570_OUT;
    f_step = KHZ_f(0, 1);
}

void radio_new_freq_ready(void)
{
    f = f_staged;
    f_staged = 0;
    state |= LCD_REDRAW;
}
