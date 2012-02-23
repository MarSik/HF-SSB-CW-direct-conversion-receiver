#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "lang.h"
#include <avr/eeprom.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"

const uint32_t dds_f = 50e6 * 10;
#define F_MAX (15e6*10) 

/*
 * State of A abn B switches in rotary encoder are combined to
 * a value according to a key [oldA, oldB, newA, newB].
 * Direction is then a function of this value.
 * Table implemented according to EMRFD, Ch. 11, page 11.7
 */ 
signed int direction(uint8_t oldstate, uint8_t newstate)
{
    uint8_t val = ((oldstate & 0b11) << 2) | (newstate & 0b11);

    switch (val) {
        /* no motion or skipped step detected */
    case 0b0000:
    case 0b0011:
    case 0b0101:
    case 0b0110:
    case 0b1001:
    case 0b1010:
    case 0b1100:
    case 0b1111:
        return 0;

        /* CCW */
    case 0b0001:
    case 0b0111:
    case 0b1000:
    case 0b1110:
        return -1;

        /* CW */
    case 0b0010:
    case 0b0100:
    case 0b1011:
    case 0b1101:
        return 1;
    }

    return 0;
}

volatile uint32_t f = 70000000;
volatile uint32_t f_step = 1000;

volatile struct _state {
    uint8_t f_changed: 1;
    uint8_t redraw: 1;
    uint8_t cw_filter: 1;
    uint8_t rotary_old: 2;
} state;

#define ROTARY_PORT PORTC
#define ROTARY_PIN PINC
#define ROTARY_DDR DDRC
#define ROTARY_BUTTON 0
#define ROTARY_A 1
#define ROTARY_B 2
#define ROTARY_SHIFT 1 // how much bits must be shifted left to get AB on the two LSB (mask 0b11)

#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_DDR DDRC
#define BUTTON_1 3
#define BUTTON_2 4
#define BUTTON_3 5
#define BUTTON_4 6

#define FILTER_PORT PORTD
#define FILTER_DDR DDRD
#define FILTER_CW 4
#define FILTER_SSB 5

void set_cw(void)
{
    PORTD |= _BV(4);
    _delay_ms(30);
    PORTD &= ~3;
    state.cw_filter = 1;
}

void set_ssb(void)
{
    PORTD |= _BV(5);
    _delay_ms(30);
    PORTD &= ~3;
    state.cw_filter = 0;
}


ISR(PCINT2_vect)
{
    if (!state.cw_filter && (BUTTON_PIN & _BV(BUTTON_1)) == 0) {
        set_cw();
        state.redraw = 1;
    }
    if (state.cw_filter && (BUTTON_PIN & _BV(BUTTON_2)) == 0) {
        set_ssb();
        state.redraw = 1;
    }

    uint8_t rotary_new = (ROTARY_PIN >> ROTARY_SHIFT) & 0b11;

    if ((ROTARY_PIN & _BV(ROTARY_BUTTON)) == 0) {
        if(f_step == 1000000) f_step = 1;
        else f_step *= 10;
    }

    f += -1 * direction(state.rotary_old, rotary_new) * f_step;

    if ((f < 1e4*10) || (f>(0xffffffff - F_MAX))) f = 1e4*10;
    else if (f>F_MAX) f = F_MAX;

    state.rotary_old = rotary_new;
    state.f_changed = 1;
    state.redraw = 1;
}

#define DDS_PORT PORTB
#define DDS_DDR DDRB
#define DDS_MISO 6
#define DDS_MOSI 5
#define DDS_SCK 7
#define DDS_CS 4

#define IR_PORT PORTB
#define IR_DDR DDRB
#define IR_DATA 2
#define IR_INT INT2

unsigned char spi(unsigned char dat) 
{ 
    unsigned char cnt, miso; 
    for (cnt = 8, miso = 0; cnt--; dat <<= 1) { 
        if (dat & 0x80) 
            DDS_PORT |= _BV(DDS_MOSI); 
        else 
            DDS_PORT &= ~_BV(DDS_MOSI);

        DDS_PORT &= ~_BV(DDS_SCK); //CLK low

        miso <<= 1; 

        if (DDS_PORT & _BV(DDS_MISO)) 
            miso |= 1; 

        DDS_PORT |= _BV(DDS_SCK); // CLK up
    }

    return miso & 0xff; 
} 

void cs_select(void)
{
    DDS_PORT &= ~_BV(DDS_CS);
}

void cs_done(void)
{
    DDS_PORT |= _BV(DDS_CS);
}

void dds_write(uint8_t cmd, uint8_t data)
{
    cs_select();
    spi(cmd);
    spi(data);
    cs_done();
}

int main(void)
{
    uint32_t dds = 0x00;
    char buffer[9];
    uint8_t i;

    DDS_DDR |= _BV(DDS_CS) | _BV(DDS_SCK) | _BV(DDS_MOSI);
    DDS_PORT |= _BV(DDS_CS) | _BV(DDS_SCK); // CS SCK idle high

    IR_DDR &= ~_BV(IR_DATA);
    IR_PORT &= ~_BV(IR_DATA);

    ROTARY_DDR &= ~(_BV(ROTARY_BUTTON) | _BV(ROTARY_A) | _BV(ROTARY_B));
    ROTARY_PORT &= ~(_BV(ROTARY_BUTTON) | _BV(ROTARY_A) | _BV(ROTARY_B));

    BUTTON_DDR &= ~(_BV(BUTTON_1) | _BV(BUTTON_2) | _BV(BUTTON_3) | _BV(BUTTON_4));
    BUTTON_PORT &= ~(_BV(BUTTON_1) | _BV(BUTTON_2) | _BV(BUTTON_3) | _BV(BUTTON_4));

    FILTER_DDR |= _BV(FILTER_CW) | _BV(FILTER_SSB) | _BV(DDS_MOSI);
    FILTER_PORT &= ~(_BV(FILTER_CW) | _BV(FILTER_SSB));

    /* rotary encoder pins are change interrupt 18, 19, 20 */
    PCICR |= _BV(PCIE2);
    PCMSK2 |= _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20);

    set_cw();

    sei();

    lcd_init();
    cs_done();

    lcd_write(s_title);
    lcd_line(1);
    lcd_write(s_author);

    //0xF800 // Reset
    dds_write(0xf8, 0x00);

    //0x303D 0x210A 0x32D7 0x2323 // Freq0
    dds_write(0x30, 0x00);
    dds_write(0x21, 0x00);
    dds_write(0x32, 0x00);
    dds_write(0x23, 0x00);

    //0x1800 0x0900 // Phase0
    dds_write(0x18, 0x00);
    dds_write(0x09, 0x00);

    //0xA000 // Sync/Selsrc
    dds_write(0x90, 0x00);

    //0xC000 // Out of reset
    dds_write(0xc0, 0x00);

    state.redraw = 1;
    lcd_clear();

    while(1) {
        while(!state.f_changed && !state.redraw);

        if(state.f_changed){
            state.f_changed = 0;
            dds = round(0xffffffff*(double)f/dds_f);

            // update DDS
            dds_write(0x30, (dds >> 0) & 0xff);
            dds_write(0x21, (dds >> 8) & 0xff);
            dds_write(0x32, (dds >> 16) & 0xff);
            dds_write(0x23, (dds >> 24) & 0xff);
        }

        if(state.redraw) {
            state.redraw = 0;
            lcd_line(0);
            
            // print Mhz
            ultoa((f/10000000), buffer, 10);
            for(i=2; i>strlen(buffer); i--) lcd_write(" ");
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
            if(state.cw_filter) lcd_write(s_cw);
            else lcd_write(s_ssb);
        }

    }

    return 0;
}
