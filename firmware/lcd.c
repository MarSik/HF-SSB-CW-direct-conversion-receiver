#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "lcd.h"

void lcd_put(uint8_t data)
{
    LCD |= _BV(EN); // set EN high
    _delay_us(100);

    LCD = (LCD & 0x70) + (data >> 4); // write high nibble

    LCDPIN |= _BV(EN); // flip EN low
    _delay_us(100);

    LCD |= _BV(EN); // set EN high
    _delay_us(100);

    LCD = (LCD & 0x70) + (data & 0x0f); // write low nibble

    LCDPIN |= _BV(EN); // strobe EN
    _delay_us(100);
}


void lcd_mode(uint8_t mode)
{
    if(mode) LCD |= _BV(RS); // RS high to data mode
    else LCD &= ~_BV(RS); // RS low to command mode
}

static void lcd_command8(uint8_t data)
{
    LCD &= ~_BV(RS); // RS low to command mode
    LCD |= _BV(EN); // set EN high
    _delay_us(100);

    LCD = (LCD & 0x70) + (data >> 4); // write high nibble

    PIND |= _BV(EN); // flip EN low
    _delay_us(100);
}

void lcd_init(void)
{
    _delay_ms(20);

    lcd_command8(0x30); //init 5ms delay
    _delay_ms(5);

    lcd_command8(0x30); //init 100us delay
    _delay_us(100);

    lcd_command8(0x30); //init 39us delay
    _delay_us(45);

    lcd_command8(0x20); //preliminary 4bit mode
    _delay_us(45);

    lcd_mode(LCD_COMMAND);
    lcd_put(0x28); //4bit mode, 2 lines 39us delay
    _delay_us(45);

    lcd_put(0x08); //display off
    _delay_us(45);

    lcd_put(0x0C); //display off
    _delay_us(45);

    lcd_put(0x01); //clear
    _delay_us(2000);

    lcd_put(0x06); //write mode (shift cursor and keep display)
    _delay_us(39);
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

void lcd_write(const unsigned char* str)
{
    lcd_mode(LCD_DATA);
    while(*str){
        lcd_put(*str);
        _delay_us(50);
        str++;
    }
}

void lcd_eep_write(const unsigned char* str)
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
