#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "lang.h"
#include <avr/eeprom.h>

#include "lcd.h"

const uint32_t dds_max EEMEM = 50e6;

int main(void)
{
    lcd_init();

    lcd_eep_write(s_title);
    lcd_line(1);
    lcd_eep_write(s_author);

    while(1) {
        /* loop */
    }

    return 0;
}
