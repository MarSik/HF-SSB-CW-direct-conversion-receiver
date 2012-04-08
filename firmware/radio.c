#include <avr/io.h>
#include <util/delay.h>

#include "state.h"
#include "radio.h"

volatile uint32_t f;
volatile uint32_t f_step;

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


void freq_offset(char dir)
{
    f += OFFSET_DIR * dir * f_step;

    if ((f < F_MIN) || (f>(0xffffffff - F_MAX))) f = F_MIN;
    else if (f>F_MAX) f = F_MAX;

    state |= F_CHANGED | LCD_REDRAW;
}

void radio_init(void)
{
    FILTER_DDR |= _BV(FILTER_CW) | _BV(FILTER_SSB);
    FILTER_PORT &= ~(_BV(FILTER_CW) | _BV(FILTER_SSB));

    set_cw();

    f = 7000000;
    f_step = 1000;

    state |= F_CHANGED | LCD_REDRAW;
}
