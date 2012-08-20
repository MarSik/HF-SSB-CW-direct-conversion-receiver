#include <stdint.h>
#include "freq.h"
#include "i2c.h"
#include "si570.h"

/* The algorithms and data structures used here are only
 * ment for CMOS version of the Si570 chip and for
 * using in QSD/QSE transceiver operating up to 30Mhz
 */

/*
 * All four following frequency vars have to have the real value divided by 4
 * to fit into the datatype and to play nicely with the QSD/QSE Johnson counters
 */
freq_t XTAL = SI570_XTAL / 4; //oscillator's XTAL frequency in Mhz / 4 (11bit.21bit)
freq_t Fdco; //PLL frequency in Mhz (11bit.21bit)
freq_t Fout; //real frequency in Mhz (11bit.21bit)

/*
 * Dividers for Si570 setup
 */
uint8_t N1; //1 and all even numbers up to 128
uint8_t HSDIV; //11,9,7,6,5,4

typedef uint16_t rfreq_t;
typedef uint32_t rfreq_f_t;

rfreq_t RFREQ_full; //10bit
rfreq_f_t RFREQ_frac; //28bit

/*
 * Recompute all dividers and RFREQ
 */
uint8_t si570_set_f(freq_t f)
{
    int i;
    uint16_t dividers = Fdco_min / f;
    if (Fdco_min % f) ++dividers; // compensate for rounding error

    // find the highest HSDIV that allows setting of the desired freq.
    HSDIV = 11;
    while(1) {
        if (HSDIV < 4) return 1; // divider error

        if (HSDIV == 10 ||
            HSDIV == 8) {
            --HSDIV;
            continue;
        }

        if ((dividers / HSDIV) > 128) {
            return 1;
        }


        // find N1 that matches the computed HSDIV
        N1 = dividers / HSDIV;
        if (dividers % HSDIV) ++N1; // compensate for rounding error

        if (N1 != 1 && N1 & 0b1) ++N1; //N1 must be even

        // recompute Fdco
        Fdco = f * HSDIV * N1;

        // check for Fdco
        // if the Fdco is too high, we might have a better
        // value with different HSDIV (better rounding)
        if (Fdco > Fdco_max) {
            --HSDIV;
            continue;
        }

        break;
    }

    RFREQ_full = Fdco / XTAL; // full part
    uint32_t rem = Fdco % XTAL; // remainder

    // compute fractional part of RFREQ
    RFREQ_frac = 0;

    // compute 28 bits, but break if the remainder is 0
    for(i = 28; (rem > 0) && (i > 0); --i) {
        rem <<= 1; //shift the remainder (manual division)
        RFREQ_frac <<= 1; //shift the fractional part to make space for new bit
        if (rem > XTAL) {
            RFREQ_frac |= 0x01;
            rem -= XTAL;
        }
    }

    // fix the shift if we exited the loop early
    RFREQ_frac <<= i;

    // save new fout
    Fout = f;

    return 0;
}

/*
 * offset RFREQ
 * checks for maximum change of 3500ppm
 */
uint8_t si570_step_f(int32_t f)
{
    return 0;
}

/*
 * Store registers to oscillator
 */
void si570_store(uint8_t freezedco)
{
    uint8_t data[7] = {
        SI570_REGISTER,
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
    uint8_t data[6] = {SI570_REGISTER, 0, 0, 0, 0, 0};

    i2c_read(SI570_ADDR, 6, data);

    HSDIV = 4 + (data[0] >> 5);
    N1 = 1 + ((data[0] & 0x1F) << 2) + (data[1] >> 6);
    RFREQ_full = ((rfreq_t)(data[1] & 0x3F) * 16) + (data[2] >> 4);
    RFREQ_frac = ((rfreq_f_t)(data[2] & 0xF) << 24) + ((rfreq_f_t)data[3] << 16) + ((rfreq_f_t)data[4] << 8) + data[5];
}

/*
 * Load all data from Si570 and compute internal
 * oscillator's frequency
 */
void si570_init(void)
{
    si570_load();
    Fdco = SI570_OUT * HSDIV * N1;
    Fout = SI570_OUT;
}

#ifdef TEST
#include <stdio.h>
void si570_print(void)
{
    uint8_t buffer[17];
    f2str(Fout, buffer, 16);
    printf("Fout : %08lx %016s\n", Fout, buffer);

    f2str(XTAL*4, buffer, 16);
    printf("XTAL : %08lx %016s\n", 4*XTAL, buffer);

    f2str(Fdco, buffer, 16);
    printf("Fdco : %08lx %016s\n", Fdco, buffer);

    f2str(Fdco_min, buffer, 16);
    printf("FdcoT: %08lx %016s\n", Fdco_min, buffer);

    f2str(Fdco_max, buffer, 16);
    printf("FdcoM: %08lx %016s\n", Fdco_max, buffer);

    printf("RFREQ: %03x %08lx %.6lf\n", RFREQ_full, RFREQ_frac, RFREQ_full + (double)RFREQ_frac/(1 << 28));
    printf("HSDIV: %d\n", HSDIV);
    printf("   N1: %d\n", N1);
}
#endif
