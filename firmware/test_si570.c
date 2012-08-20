#include <stdlib.h>
#include <stdio.h>

#include "i2c.h"
#include "si570.h"

extern uint32_t Fdco; //PLL frequency in Mhz (11bit.21bit)
extern uint8_t N1; //1 and all even numbers up to 128
extern uint8_t HSDIV; //11,9,7,6,5,4
extern uint16_t RFREQ_full; //10bit
extern uint32_t RFREQ_frac; //28bit

uint8_t *test_data = NULL;

int i2c_read(uint8_t slave, uint8_t count, uint8_t *data)
{
    uint8_t reg = *data;
    uint8_t oldcount = count;

    uint8_t *p = test_data;
    while (count) {
        *data = *p;
        data++;
        p++;
        count--;
    }

    return oldcount;
}

int i2c_write(uint8_t slave, uint8_t count, uint8_t *data)
{
    uint8_t *p = test_data;
    uint8_t oldcount = count;

    while (count) {
        *p = *data;
        data++;
        p++;
        count--;
    }

    return oldcount;
}

uint8_t get_oscfreq_data[] = {0xE1, 0xC2, 0xB5, 0xD2, 0x2B, 0xD7};

void test_get_oscfreq(void)
{
    test_data = get_oscfreq_data;
    si570_init();

    if(HSDIV != 11 ||
       N1 != 8 ||
       Fdco != 0x9ae147a0 ||
       RFREQ_full != 0x02b ||
       RFREQ_frac != 0x5d22bd7) {
        printf("=== error in test_get_oscfreq ===\n");
        si570_print();
    }
}

void test_set_oscfreq(void)
{
    uint8_t buffer[7];
    uint8_t *p = get_oscfreq_data;
    uint8_t *p2 = buffer;
    uint8_t i;

    test_data = get_oscfreq_data;
    si570_init();
    test_data = buffer;
    si570_store(0);

    if(*p2 != SI570_REGISTER) {
        printf("=== error in test_set_oscfreq ===\nbad address %x\n", *p2);
        return;
    }
    p2++;

    i = 6;
    while (i>0) {
        if (*p2 != *p) {
            printf("=== error in test_set_oscfreq ===\n");
            break;
        }
        --i;
        p2++;
        p++;
    }
}

void test_new_oscfreq(void)
{
    test_data = get_oscfreq_data;
    si570_init();
    si570_set_f(0x1c28f5c);

    if(HSDIV != 11 ||
       N1 != 8 ||
       Fdco != 0x9ae147a0 ||
       RFREQ_full != 0x02b ||
       RFREQ_frac != 0x05d22bf7) {
        printf("=== error in test_new_oscfreq ===\n");
        si570_print();
    }
}

void test_f(uint32_t f)
{
    uint8_t res;

    test_data = get_oscfreq_data;
    si570_init();
    res = si570_set_f(f);

    if (Fdco < Fdco_min ||
        Fdco > Fdco_max ||
        HSDIV < 4 ||
        HSDIV == 8 ||
        HSDIV == 10 ||
        HSDIV > 11 ||
        N1 > 128 ||
        N1 < 1 ||
        (N1 != 1 && N1 % 2)) {
        if (res == 0) {
            printf("=== error in test_f: %lx %.8lf ===\n", f, (double)f / (1 << 21));
            si570_print();
        }
    }
}


int main(void)
{
    uint32_t f;

    test_get_oscfreq();
    test_new_oscfreq();
    test_set_oscfreq();

    for(f = (3 << 21); f < (31 << 21); ++f)
        test_f(f);

    return 0;
}
