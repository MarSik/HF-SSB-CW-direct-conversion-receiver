#include <avr/io.h>
#include <inttypes.h>
#include "config.h"
#include "serial.h"

void serial_init()
{
    // Initialize both hw serial ports

#ifdef SERIAL1_ENABLE
    /* UART0 is shared with bootloader
       use the same settings
    */
    UBRR0L = (uint8_t)(F_CPU/(BAUD_RATE0*16L)-1);
    UBRR0H = (F_CPU/(BAUD_RATE0*16L)-1) >> 8;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

    /* Enable internal pull-up resistor on RX pins,
       in order to supress line noise.
    */
    SERIAL1_DDR &= ~_BV(SERIAL1_RX);
    SERIAL1_PORT |= _BV(SERIAL1_RX);
#endif

#ifdef SERIAL2_ENABLE
    /* UART1
    */
    UBRR1L = (uint8_t)(F_CPU/(BAUD_RATE1*16L)-1);
    UBRR1H = (F_CPU/(BAUD_RATE1*16L)-1) >> 8;
    UCSR1B = _BV(RXEN1) | _BV(TXEN1);
    UCSR1C = _BV(UCSZ10) | _BV(UCSZ11);

    /* Enable internal pull-up resistor on RX pins,
       in order to supress line noise.
    */
    SERIAL2_DDR &= ~_BV(SERIAL2_RX);
    SERIAL2_PORT |= _BV(SERIAL2_RX);
#endif

}

void serial0_putch(uint8_t ch)
{
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = ch;
}

void serial0_puts(uint8_t *s)
{
    while(*s != 0) serial0_putch(*s++);
}

void serial1_putch(uint8_t ch)
{
    while (!(UCSR1A & _BV(UDRE1)));
    UDR1 = ch;
}

void serial1_puts(uint8_t *s)
{
    while(*s != 0) serial1_putch(*s++);
}
