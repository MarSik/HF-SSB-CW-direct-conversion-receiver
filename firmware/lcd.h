#ifndef LCD_H_2012_0203_1436_MarSik_ASDFKJAS
#define LCD_H_2012_0203_1436_MarSik_ASDFKJAS

/*
 * LCD configuration
 * 
 * all data & control lines have to be connected to
 * one port (PORTX) on the mcu
 */

#define LCD PORTA
#define LCD_PIN PINA
#define LCD_DDR DDRA

/* LCD data lines are connected to
 *        D4 -- PORTX0
 *        D5 -- PORTX1
 *        D6 -- PORTX2
 *        D7 -- PORTX3
 *
 * control lines are connected to Px (defined below)
 *  (RW is tied to GND)
 */
#define RS 5
#define EN 4

/*
 * LCD constants
 */
#define LCD_COMMAND 0
#define LCD_DATA 1

/* send one character/command byte to LCD */
void lcd_put(uint8_t data);

/* change current mode to LCD_COMMAND or LCD_DATA */
void lcd_mode(uint8_t mode);

/* reinitialize LCD */
void lcd_init(void);

/* clear LCD */
void lcd_clear(void);

/* move cursor to beginning of <line>. line
   numbering starts at 0 */
void lcd_line(uint8_t line);

/* send DATA string from RAM to LCD */
void lcd_write(const unsigned char* str);

/* send DATA string from EEPROM to LCD */
void lcd_eep_write(const unsigned char* str);

#endif /* LCD_H_2012_0203_1436_MarSik_ASDFKJAS */
