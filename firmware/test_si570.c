#include <stdlib.h>
#include <stdio.h>

#include "i2c.h"
#include "si570.h"

uint8_t *test_data = NULL;

int i2c_read(uint8_t slave, uint8_t count, uint8_t *data)
{
    uint8_t reg = *data;

    uint8_t *p = test_data;
    while (count) {
        *data = *p;
        data++;
        p++;
        count--;
    }

    return count;
}

int i2c_write(uint8_t slave, uint8_t count, uint8_t *data)
{
    while(count--) {
        printf(" %02x", *data);
        data++;
    }

    printf("\n");

    return count;
}

uint8_t get_oscfreq_data[] = {0xE1, 0xC2, 0xB5, 0xD2, 0x2B, 0xD7};

void test_get_oscfreq(void)
{
    test_data = get_oscfreq_data;
    si570_init();
    si570_print();
}

void test_set_oscfreq(void)
{
    test_data = get_oscfreq_data;
    si570_init();
    si570_store(0);
}

void test_new_oscfreq(void)
{
    test_data = get_oscfreq_data;
    si570_init();
    si570_set_f(SI570_OUT);
    si570_print();
}

int main(void)
{
    test_get_oscfreq();
    test_new_oscfreq();
    test_set_oscfreq();
    return 0;
}
