#include <stdlib.h>
#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "lang.h"
#include "radio.h"

static volatile uint8_t address = 0x00;
static volatile uint8_t *p; // pointer to buffer data

static volatile uint8_t reg;
static volatile uint8_t count;

static volatile i2c_callback cb;
static volatile void *cb_data;

ISR(TWI_vect)
{
    if (TW_STATUS == TW_START) {
        // send address masked for write
        TWDR = address & (~1);
        TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);
    }

    else if (TW_STATUS == TW_MT_SLA_ACK) {
        TWDR = reg;
        TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);
    }

    else if (TW_STATUS == TW_MT_DATA_ACK) {
        // read requested, restart in MR mode
        if (address & 0x1) {
            TWCR = (1<<TWINT) | _BV(TWIE) | _BV(TWSTA) | _BV(TWEN);
            return;
        }

        if (!count) {
            TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN) | (1<<TWSTO);
            address = 0x00;
            if (cb) cb(cb_data, TW_STATUS);
            return;
        }

        TWDR = *p;
        TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);

        ++p;
        --count;
    }

    else if (TW_STATUS == TW_REP_START) {
        // send address + 1 bit for read
        TWDR = address;
        TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);
    }

    else if (TW_STATUS == TW_MR_SLA_ACK) {
        --count;

        if (count) TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN) | (1<<TWEA);
        else TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);
    }

    else if (TW_STATUS == TW_MR_DATA_ACK) {
        *p = TWDR;
        ++p;

        --count;
        if (count) TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN) | (1<<TWEA);
        else TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);
    }

    else if (TW_STATUS == TW_MR_DATA_NACK) {
        TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN) | (1<<TWSTO);
        address = 0x00;
        if (cb) cb(cb_data, TW_STATUS);
    }

    // report errors
    else {
        radio_set_error(s_i2c_error, TW_STATUS);
    }
}

void i2c_init(void)
{
    DDRC &=  (~0b11); //SDA. SCL as inputs
    PORTC |= 0b11; // pullups on SDA and SCL
}

uint8_t i2c_status(void)
{
    return TWSR;
}

int i2c_transfer(const uint8_t slave, const uint8_t start_reg,
                 uint8_t *buffer, const uint8_t data_count,
                 const i2c_callback cb0, void *cb_data0)
{
    if (address) return -1;

    address = slave;
    reg = start_reg;
    p = buffer;
    count = data_count;
    cb = cb0;
    cb_data = cb_data0;

    TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWSTA) | _BV(TWEN);

    return 0;
}

