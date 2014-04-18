#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "lcd.h"

static const uint8_t lcd_c_cw[] PROGMEM = {0x1f, 0x1f, 0xe, 0x6, 0xa, 0xc, 0xe, 0x1f, 0};
static const uint8_t lcd_c_ssb[] PROGMEM = {0x1f, 0x1f, 0xe, 0xa, 0xa, 0xa, 0x15, 0x1f, 0};
static const uint8_t lcd_c_tx[] PROGMEM = {0x1f, 0x15, 0x1b, 0x15, 0x1b, 0x1b, 0x11, 0x1f, 0};

void lcd_put(uint8_t data)
{
    LCD |= _BV(EN); // set EN high
    _delay_us(100);

    LCD = (LCD & 0xF0) + ((data >> 4) & 0x0f); // write high nibble

    LCD_PIN |= _BV(EN); // flip EN low
    _delay_us(100);

    LCD |= _BV(EN); // set EN high
    _delay_us(100);

    LCD = (LCD & 0xF0) | (data & 0x0f); // write low nibble

    LCD_PIN |= _BV(EN); // strobe EN
    _delay_us(100);
}


void lcd_mode(uint8_t mode)
{
    if(mode == LCD_DATA) LCD |= _BV(RS); // RS high to data mode
    else LCD &= ~_BV(RS); // RS low to command mode
}

static void lcd_command8(uint8_t data)
{
    lcd_mode(LCD_COMMAND); // LCD to command mode

    LCD |= _BV(EN); // set EN high
    _delay_us(100);

    LCD = (LCD & 0xF0) | ((data >> 4) & 0x0f); // write high nibble

    LCD_PIN |= _BV(EN); // flip EN low
    _delay_us(100);
}

void lcd_init(void)
{
    // set the port to proper direction
    LCD &= ~(0x0f | _BV(EN) | _BV(RS));
    LCD_DDR |= 0x0f | _BV(EN) | _BV(RS);

    _delay_ms(20);

    lcd_mode(LCD_COMMAND);

    lcd_command8(0x30); //init 5ms delay
    _delay_ms(5);

    lcd_command8(0x30); //init 100us delay
    _delay_us(100);

    lcd_command8(0x30); //init 39us delay
    _delay_us(45);

    lcd_command8(0x20); //preliminary 4bit mode
    _delay_us(45);

    lcd_put(0x28); //4bit mode, 2 lines 39us delay
    _delay_us(45);

    lcd_put(0x08); //display off
    _delay_us(45);

    lcd_put(0x0C); //display on
    _delay_us(45);

    lcd_put(0x01); //clear
    _delay_us(2000);

    lcd_put(0x06); //write mode (shift cursor and keep display)
    _delay_us(39);

    /* load new characters */
    /*
    static const uint8_t l_cw[] EEMEM = {0x1f, 0x1f, 0xe, 0x6, 0xa, 0xc, 0xe, 0x1f};
    static const uint8_t l_ssb[] EEMEM = {0x1f, 0x1f, 0xe, 0xa, 0xa, 0xa, 0x15, 0x1f};
    static const uint8_t l_tx[] EEMEM = {0x1b, 0xa, 0x15, 0x1f, 0x1b, 0x1b, 0x1b, 0x11};
    */

    lcd_newchar(0);
    lcd_pgm_write(lcd_c_cw);

    lcd_newchar(1);
    lcd_pgm_write(lcd_c_ssb);

    lcd_newchar(2);
    lcd_pgm_write(lcd_c_tx);

    lcd_line(0);
    lcd_mode(LCD_DATA);
}

void lcd_clear(void)
{
    lcd_mode(LCD_COMMAND);
    lcd_put(0x01); //2000us delay
    _delay_us(2000);
}

void lcd_line(uint8_t line)
{
    lcd_mode(LCD_COMMAND);
    lcd_put(0x80+line*0x40);
    _delay_us(45);
}

void lcd_write(const uint8_t* str)
{
    lcd_mode(LCD_DATA);

    while(*str){
        lcd_put(*str);
        _delay_us(50);
        str++;
    }
}

void lcd_eep_write(const uint8_t* str)
{
    uint8_t c;

    lcd_mode(LCD_DATA);
    while(1) {
        c = eeprom_read_byte(str);
        if(c==0) return;

        lcd_put(c);
        _delay_us(50);
        str++;
    }
}

void lcd_pgm_write(const uint8_t* str)
{
    uint8_t c;

    lcd_mode(LCD_DATA);
    while(1) {
        c = pgm_read_byte(str);
        if(c==0) return;

        lcd_put(c);
        _delay_us(50);
        str++;
    }
}

void lcd_newchar(uint8_t idx)
{
    lcd_mode(LCD_COMMAND);
    lcd_put(0x40 + (idx * 8));
    _delay_us(45);
}
