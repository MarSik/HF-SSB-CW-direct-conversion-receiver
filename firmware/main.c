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

int main(void)
{
    char buffer[9];
    uint8_t i;
    uint8_t fl;
    const uint8_t *extra;

    cli();

    /* disable ADC */
    ACSR |= _BV(ACD);
    ADCSRA &= ~_BV(ADEN);

    lcd_init();
    ir_init();
    interface_init();
    radio_init();
    dds_init();
    
    sei();

    lcd_eep_write(s_title);
    lcd_line(1);
    lcd_eep_write(s_author);


    state |= LCD_REDRAW;
    _delay_ms(1000);
    lcd_clear();

    while(1) {
        if(state & F_CHANGED){
            fl = bandplan(f / 1000, &extra);
            state &= ~F_CHANGED;

            // update DDS
            dds_f1(f);
        }

        if(state & LCD_REDRAW) {
            state.redraw = 0;
            state &= ~LCD_REDRAW;
            lcd_line(0);
            
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
            lcd_put(' ');
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
            if(state & ST_CW) lcd_eep_write(s_cw);
            else lcd_eep_write(s_ssb);
        }

        /* sleep the cpu to minimize RF noise */
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
    }

    return 0;
}
