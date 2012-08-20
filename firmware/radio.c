#include <avr/io.h>
#include <util/delay.h>

#include "state.h"
#include "radio.h"
#include "freq.h"

volatile freq_t f;
volatile freq_t f_step;

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
    f += OFFSET_DIR * dir * f_step;

    if ((f < F_MIN) || (f>(0xffffffff - F_MAX))) f = F_MIN;
    else if (f>F_MAX) f = F_MAX;

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

    f = MHZ_f(7);
    f_step = KHZ_f(0, 1);

    state |= F_CHANGED | LCD_REDRAW;
}
