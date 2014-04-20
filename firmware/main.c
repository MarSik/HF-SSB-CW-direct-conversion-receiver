#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <math.h>
#include <stdlib.h>
#include "lang.h"
#include "lcd.h"
#include "bandplan.h"
#include "ir.h"
#include "interface.h"
#include "i2c.h"
#include "spi.h"
#include "tuner.h"
#include "state.h"
#include "radio.h"
#include "freq.h"

int main(void)
{
    cli();

    /* disable ADC */
    ACSR |= _BV(ACD);
    ADCSRA &= ~_BV(ADEN);

    ir_init();
    interface_init();
    radio_init();
    spi_init();
    i2c_init();
    sei();

    //si570_init();
    tuner_init();
    //dds_init();

    state |= LCD_REDRAW;
    lcd_init();
    lcd_clear();

    while(1) {
        if (state & IR_DATA_READY) {
            state &= ~IR_DATA_READY;

            uint8_t ird = ir_get();
            if (ird == A_PWR) {
                lcd_clear();
                /* power button */
            }
            else if (ird == A_CHUP) {
                freq_step(F_DIR_UP);
                state |= LCD_REDRAW;
            }
            else if (ird == A_CHDOWN) {
                freq_step(F_DIR_DOWN);
                state |= LCD_REDRAW;
            }
            else if (ird == A_VUP) {
                step_up();
                state |= LCD_REDRAW;
            }
            else if (ird == A_VDOWN) {
                step_down();
                state |= LCD_REDRAW;
            }
            else if (ird == A_B1) {
                /* button 1 */
            }
            else {
                lcd_clear();
                lcd_line(0);
                utoa(ird, buffer, 16);
                lcd_pgm_write(s_ircode);
                lcd_write((unsigned char*)buffer);
            }
        }

        if (state & F_CHANGED){
            state &= ~F_CHANGED;
            state |= LCD_REDRAW;

            tuning_record record;
            if (tuner_find(f, &record)) {
                tuner_set_from_record(&record);
            }
        }

        if (state & LCD_CLEAR) {
            state &= ~LCD_CLEAR;
            lcd_clear();
        }

        if (state & LCD_REDRAW) {
            state &= ~LCD_REDRAW;
            interface_render();
        }

        /* sleep the cpu to minimize RF noise */
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }

    return 0;
}
