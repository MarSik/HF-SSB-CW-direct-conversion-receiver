#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "lcd.h"

/* Custom characters
   7 character lines
   1 cursor line (displayed after ORed with cursor)
   5 bits per line

   0 bit = dark
   1 bit = light
*/
static const uint8_t lcd_c_mem[] PROGMEM = {0x1f, 0x1f, 0x0e, 0x04, 0x0a, 0x0e, 0x0e,
                                            0x1f};
static const uint8_t lcd_c_up[] PROGMEM = {0x04, 0x0e, 0x1f, 0x04, 0x04, 0x04, 0x00,
                                           0x00};

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
    lcd_newchar(0);
    lcd_pgm_write_len(lcd_c_mem, 8);

    lcd_newchar(1);
    lcd_pgm_write_len(lcd_c_up, 8);

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

void lcd_pgm_write_len(const uint8_t* str, uint8_t len)
{
    uint8_t c;

    lcd_mode(LCD_DATA);
    while(len--) {
        c = pgm_read_byte(str);
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
