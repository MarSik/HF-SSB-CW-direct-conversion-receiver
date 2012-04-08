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

#define F_MAX (15e6*10)
#define F_MIN (1e4*10)


#define FILTER_PORT PORTD
#define FILTER_DDR DDRD
#define FILTER_CW 5
#define FILTER_SSB 4

volatile uint32_t f = 70000000;
volatile uint32_t f_step = 1000;

volatile struct _state {
    uint8_t f_changed: 1;
    uint8_t redraw: 1;
    uint8_t cw_filter: 1;
} state;

void set_cw(void)
{
    FILTER_PORT |= _BV(FILTER_CW);
    _delay_ms(30);
    FILTER_PORT &= ~_BV(FILTER_CW);
    state.cw_filter = 1;
}

void set_ssb(void)
{
    FILTER_PORT |= _BV(FILTER_SSB);
    _delay_ms(30);
    PORTD &= ~_BV(FILTER_SSB);
    state.cw_filter = 0;
}


ISR(PCINT2_vect)
{
    f += ROTARY_DIR * direction(state.rotary_old, rotary_new) * f_step;

    if ((f < F_MIN) || (f>(0xffffffff - F_MAX))) f = F_MIN;
    else if (f>F_MAX) f = F_MAX;

    state.f_changed = 1;
    state.redraw = 1;
}


int main(void)
{
    char buffer[9];
    uint8_t i;
    uint8_t fl;
    const uint8_t *extra;

    cli();


    FILTER_DDR |= _BV(FILTER_CW) | _BV(FILTER_SSB) | _BV(DDS_MOSI);
    FILTER_PORT &= ~(_BV(FILTER_CW) | _BV(FILTER_SSB));

    /* disable ADC */
    ACSR |= _BV(ACD);
    ADCSRA &= ~_BV(ADEN);

    set_cw();
    lcd_init();
    ir_init();
    interface_init();
    dds_init();
    
    sei();

    lcd_eep_write(s_title);
    lcd_line(1);
    lcd_eep_write(s_author);


    state.redraw = 1;
    _delay_ms(1000);
    lcd_clear();

    while(1) {
        if(state.f_changed){
            fl = bandplan(f / 1000, &extra);
            state.f_changed = 0;

            // update DDS
            dds_f1(f);
        }

        if(state.redraw) {
            state.redraw = 0;
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
            if(state.cw_filter) lcd_eep_write(s_cw);
            else lcd_eep_write(s_ssb);
        }

        /* sleep the cpu to minimize RF noise */
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
        
    }

    return 0;
}
