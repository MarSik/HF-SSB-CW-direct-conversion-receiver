#include <stdlib.h>
#include <avr/io.h>
#include <util/twi.h>

int i2c_write_raw(uint8_t slave, uint8_t count, uint8_t *data)
{
    uint8_t sent = 0;

    // send start bit
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // wait for the bus
    while (!(TWCR & (1<<TWINT)))
        ;

    // check if the start bit was transmitted successfully
    if ((TWSR & 0xF8) != TW_START)
        return -1;

    // send address + 0 bit for write
    TWDR = slave << 1;
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait for the bus
    while (!(TWCR & (1<<TWINT)))
        ;

    // check for errors
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
        return -2;

    while (count > sent) {
        TWDR = *data;
        TWCR = (1<<TWINT) | (1<<TWEN);

        while (!(TWCR & (1<<TWINT)))
            ;

        if ((TWSR & 0xF8) !=TW_MT_DATA_ACK)
            break;

        sent++;
        data++;
    }

    return sent;
}

int i2c_write(uint8_t slave, uint8_t count, uint8_t *data)
{
    int sent = i2c_write_raw(slave, count, data);

    // stop condition
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return sent;
}

int i2c_read(uint8_t slave, uint8_t count, uint8_t *data)
{
    int read = i2c_write_raw(slave, 1, data);

    // send start bit
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // wait for the bus
    while (!(TWCR & (1<<TWINT)))
        ;

    // check if the start bit was transmitted successfully
    if ((TWSR & 0xF8) != TW_START)
        return -1;

    // send address + 1 bit for read
    TWDR = 1 + (slave << 1);
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait for the bus
    while (!(TWCR & (1<<TWINT)))
        ;

    // check for errors
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
        return -2;

    while (count > read) {
        if (count == read + 1)
            TWCR = (1<<TWINT);
        else
            TWCR = (1<<TWINT) | (1<<TWEA);

        while (!(TWCR & (1<<TWINT)))
            ;

        if (((TWSR & 0xF8) != 0x50) ||
            ((TWSR & 0xF8) != 0x58))
            break;

        *data = TWDR;

        read++;
        data++;
    }

    return read;
}
