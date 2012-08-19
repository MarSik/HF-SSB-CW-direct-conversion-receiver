#include <stdint.h>
#include "i2c.h"
#include "si570.h"

/*
 * All four following frequency vars have to have the real value divided by 4
 * to fit into the datatype and to play nicely with the QSD/QSE Johnson counters
 */
uint32_t XTAL = SI570_XTAL / 4; //oscillator's XTAL frequency in Mhz / 4 (11bit.21bit)
uint32_t Fdco; //PLL frequency in Mhz (11bit.21bit)
#define Fdco_tgt ((uint32_t)4900 << (21 - 2)) // target PLL freq 4900Mhz / 4
uint32_t Fout; //real frequency in Mhz (11bit.21bit)

/*
 * Dividers for Si570 setup
 */
uint8_t N1; //1 and all even numbers up to 128
uint8_t HSDIV; //11,9,7,6,5,4
uint16_t RFREQ_full; //10bit
uint32_t RFREQ_frac; //28bit

/*
 * Recompute all dividers and RFREQ
 */
uint8_t si570_set_f(uint32_t f)
{
    uint16_t dividers = Fdco_tgt / f;

    // find the highest HSDIV that allows setting of the desired freq.
    HSDIV = 11;
    while ((dividers / HSDIV) > 128) {
        --HSDIV;

        if (HSDIV == 10 ||
            HSDIV == 8) {
            --HSDIV;
            continue;
        }

        if (HSDIV < 4) return 1; // divider error
    }

    // find N1 that matches the computed HSDIV
    N1 = dividers / HSDIV;
    if (dividers % HSDIV) ++N1; // we need to round up in the fractional case

    // recompute dividers to account for rounding errors)
    dividers = HSDIV * N1;
    Fdco = f * dividers;

    RFREQ_full = Fdco / XTAL; // full part
    uint32_t rem = Fdco % XTAL; // remainder

    // compute fractional part of RFREQ
    RFREQ_frac = 0;
    int i;

    // compute 28 bits, but break if the remainder is 0
    for(i = 28; (rem > 0) && (i > 0); --i) {
        rem <<= 1; //shift
        RFREQ_frac <<= 1;
        if (rem > XTAL) {
            RFREQ_frac |= 0x01;
            rem -= XTAL;
        }
    }

    // fix the shift if we exited the loop early
    RFREQ_frac <<= i;

    return 0;
}

/*
 * offset RFREQ
 * checks for maximum change of 3500ppm
 */
uint8_t si570_step_f(int16_t f)
{
}

/*
 * Store registers to oscillator
 */
void si570_store(uint8_t freezedco)
{
    uint8_t data[7] = {
        7,
        ((HSDIV - 4) << 5) + ((N1 - 1) >> 2),
        (((N1 - 1) & 0x3) << 6) + (RFREQ_full >> 4),
        ((RFREQ_full & 0xF) << 4) + ((RFREQ_frac) >> 24),
        (RFREQ_frac >> 16) & 0xff,
        (RFREQ_frac >> 8) & 0xff,
        RFREQ_frac & 0xff
    };

    i2c_write(SI570_ADDR, 7, data);
}

/*
 * Load registers from oscillator
 */
void si570_load(void)
{
    uint8_t data[6] = {0x7, 0, 0, 0, 0, 0};

    i2c_read(SI570_ADDR, 6, data);

    HSDIV = 4 + (data[0] >> 5);
    N1 = 1 + ((data[0] & 0x1F) << 2) + (data[1] >> 6);
    RFREQ_full = ((data[1] & 0x3F) * 16) + (data[2] >> 4);
    RFREQ_frac = ((data[2] & 0xF) << 24) + (data[3] << 16) + (data[4] << 8) + data[5];
}

/*
 * Load all data from Si570 and compute internal
 * oscillator's frequency
 */
void si570_init(void)
{
    si570_load();
    Fdco = SI570_OUT * HSDIV * N1;
}

#ifdef TEST
#include <stdio.h>
void si570_print(void)
{
    printf("XTAL : %08lx %lf\n", XTAL, (double)XTAL / (1 << 21));
    printf("Fdco : %08lx %lf\n", Fdco, (double)Fdco / (1 << 21));
    printf("FdcoT: %08lx %lf\n", Fdco_tgt, (double)Fdco_tgt / (1 << 21));
    printf("RFREQ: %03x %08lx %lf\n", RFREQ_full, RFREQ_frac, RFREQ_full + (double)RFREQ_frac/(1 << 28));
    printf("HSDIV: %d\n", HSDIV);
    printf("   N1: %d\n", N1);
}
#endif
