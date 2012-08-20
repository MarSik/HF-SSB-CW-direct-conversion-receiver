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
#include "spi.h"
#include "dds.h"
#include "state.h"
#include "radio.h"

int main(void)
{
    char buffer[9];
    uint8_t i;
    uint8_t fl = 0;
    const uint8_t *extra = NULL;

    cli();

    /* disable ADC */
    ACSR |= _BV(ACD);
    ADCSRA &= ~_BV(ADEN);

    lcd_init();
    ir_init();
    interface_init();
    radio_init();
    spi_init();
    dds_init();
    sei();

    lcd_eep_write(s_title);
    lcd_line(1);
    lcd_eep_write(s_author);
    _delay_ms(1000);

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
            fl = bandplan(f_sf(f), &extra);
            state &= ~F_CHANGED;

            // update DDS frequency
            dds_f1(f);
        }

        if (state & LCD_REDRAW) {
            state &= ~LCD_REDRAW;
            lcd_line(0);
            lcd_mode(LCD_DATA);

            // print Mhz
            ultoa((f/10000000), buffer, 10);
            for(i=2; i>strlen(buffer); i--) lcd_put(' ');
            lcd_write((unsigned char*)buffer);
            lcd_put('.');

            // print kHz
            ultoa((f % 10000000) / (10000), buffer, 10);
            for(i=3; i>strlen(buffer); i--) lcd_put('0');
            lcd_write((unsigned char*)buffer);
            lcd_put('.');

            // print Hz
            ultoa((f % 10000) / 10, buffer, 10);
            for(i=3; i>strlen(buffer); i--) lcd_put('0');
            lcd_write((unsigned char*)buffer);

            // print step size
            if (state & ST_CW) lcd_put(0x00);
            else lcd_put(0x01);

            lcd_put(' ');
            if (f_step<1000) {
                utoa(f_step, buffer, 10);
                for(i=4; i>strlen(buffer); i--) lcd_put(' ');
                lcd_write((unsigned char*)buffer);
            }
            else if (f_step<1000000) {
                utoa(f_step/1000, buffer, 10);
                for(i=3; i>strlen(buffer); i--) lcd_put(' ');
                lcd_write((unsigned char*)buffer);
                lcd_put('k');
            }
            else {
                utoa(f_step/1000000, buffer, 10);
                for(i=3; i>strlen(buffer); i--) lcd_put(' ');
                lcd_write((unsigned char*)buffer);
                lcd_put('M');
            }

            lcd_line(1);
            lcd_mode(LCD_DATA);

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

        /* sleep the cpu to minimize RF noise */
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }

    return 0;
}
