#ifndef LCD_H_2012_0203_1436_MarSik_ASDFKJAS
#define LCD_H_2012_0203_1436_MarSik_ASDFKJAS

#include <inttypes.h>

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
void lcd_write(const uint8_t* str);

/* send DATA string from EEPROM to LCD */
void lcd_eep_write(const uint8_t* str);

/* send DATA string from PROGMEM to LCD */
void lcd_pgm_write(const uint8_t* str);
void lcd_pgm_write_len(const uint8_t* str, uint8_t len);

/* prepare lcd for receiving a new character
   id must be 0 to 7
   and this call must be followed by 8 bytes with bitmask
*/
void lcd_newchar(uint8_t idx);

#endif /* LCD_H_2012_0203_1436_MarSik_ASDFKJAS */
