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
#include "dds.h"
#include "si570.h"
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

    lcd_eep_write(s_title);
    lcd_line(1);
    lcd_eep_write(s_author);
    _delay_ms(1000);

    ir_init();
    interface_init();
    radio_init();

    i2c_init();
    sei();

    si570_init();
    //spi_init();
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
                lcd_eep_write(s_ircode);
                lcd_write((unsigned char*)buffer);
            }
        }

        if (state & F_SMALL_CHANGE){
            state &= ~F_SMALL_CHANGE;
            state |= F_CHANGED;
        }

        if (state & F_CHANGED){
            state &= ~F_CHANGED;

            // update DDS frequency
            //dds_f1(f);
            int big = si570_set_f(f_staged);
            if (big >= 0) {
                si570_store(big);
                state |= LCD_REDRAW;
            }
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

                // print filter size
                if (state & ST_CW) lcd_put(0x00);
                else lcd_put(0x01);

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
            else lcd_eep_write(s_initializing);

            lcd_line(1);
            lcd_mode(LCD_DATA);

            if (error) {
                    lcd_eep_write(error);
                    utoa(error_id, buffer, 16);
                    lcd_write(buffer);
            }
            else if (f>0) {
                fl = bandplan(f_sf(f), &extra);

                if (fl & CW) {
                    lcd_eep_write(s_cw);
                    lcd_put(' ');
                }

                if (fl & SSB) {
                    lcd_eep_write(s_ssb);
                    lcd_put(' ');
                }

                if (fl & DIGI) {
                    lcd_eep_write(s_digi);
                    lcd_put(' ');
                }

                if (TXOK(fl)) lcd_put(0x2);

                if (extra) lcd_eep_write(extra);
                else lcd_write("        ");
            }
        }

        /* sleep the cpu to minimize RF noise */
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }

    return 0;
}
