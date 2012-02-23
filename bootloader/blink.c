#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRA |= _BV(7);

    while(1) {
	PINA |= _BV(7);
	_delay_ms(200);
    }
}

