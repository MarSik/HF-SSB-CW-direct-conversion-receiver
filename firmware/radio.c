#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "state.h"
#include "radio.h"
#include "freq.h"

volatile freq_t f;
volatile freq_t f_step;

volatile char *error = NULL;
volatile uint8_t error_id;

void radio_set_error(char *eep_string, uint8_t id)
{
    error = eep_string;
    error_id = id;
    state |= LCD_REDRAW;
}

void freq_step(signed char dir)
{
    freq_t f_staged = f + OFFSET_DIR * dir * f_step;

    if ((f_staged < F_MIN) || (f_staged>(0xffffffff - F_MAX))) f_staged = F_MIN;
    else if (f_staged>F_MAX) f_staged = F_MAX;

    f = f_staged;
    state |= F_CHANGED;
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
    f = MHZ_f(7);
    f_step = KHZ_f(0, 1);
}
