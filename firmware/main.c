#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
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
    uint8_t buffer[9];
    uint8_t i;
    uint8_t fl = 0;
    const uint8_t *extra = NULL;

    cli();

    /* disable ADC */
    ACSR |= _BV(ACD);
    ADCSRA &= ~_BV(ADEN);

    lcd_init();

    lcd_pgm_write(s_title);
    lcd_line(1);
    lcd_pgm_write(s_author);
    _delay_ms(1000);

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
        }

        if (state & LCD_REDRAW) {
            state &= ~LCD_REDRAW;
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
                    for(i=3; i>strlen(buffer); i--) lcd_put(' ');
                    lcd_write(buffer);
                    lcd_put('M');
                }
                else if (f_KHZ(f_step)) {
                    utoa(f_KHZ(f_step), buffer, 10);
                    for(i=3; i>strlen(buffer); i--) lcd_put(' ');
                    lcd_write(buffer);
                    lcd_put('k');
                }
                else {
                    utoa(f_HZ(f_step), buffer, 10);
                    for(i=4; i>strlen(buffer); i--) lcd_put(' ');
                    lcd_write(buffer);
                }

            }
            else lcd_pgm_write(s_initializing);

            lcd_line(1);
            lcd_mode(LCD_DATA);

            if (error) {
                    lcd_pgm_write(error);
                    utoa(error_id, buffer, 16);
                    lcd_write(buffer);
            }
            else if (f>0) {
                fl = bandplan(f_sf(f), &extra);

                uint16_t v = tuner_get_real_l();
                uint8_t r = 0;
                while(v > 1000) {
                    v /= 1000;
                    r++;
                }

                utoa(v, buffer, 10);
                for(i=4; i>strlen(buffer); i--) lcd_put(' ');
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
                for(i=4; i>strlen(buffer); i--) lcd_put(' ');
                lcd_write(buffer);
                lcd_put(PICO[r]);
                lcd_put('F');
            }
        }

        /* sleep the cpu to minimize RF noise */
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }

    return 0;
}
