#ifndef _FREQ_H_
#define _FREQ_H_

#include <stdint.h>

/* All frequency methods accept frequency in fixed point with 21 fractional bits,
   so we can represent fractions with half a hertz precision */
typedef uint32_t freq_t;
typedef uint16_t sfreq_t;

#define MHZ_f(m) ((freq_t)(m) << 21)
#define KHZ_f(m, k) (MHZ_f(m) + MHZ_f(k)/1000)
#define HZ_f(m, k, h) (KHZ_f(m, k) + MHZ_f(h)/1000000)

#define f_MHZ(f) ((uint16_t)((f) >> 21))
#define f_KHZ(f) ((freq_t)(f) & (freq_t)0x1fffff)

// short frequency is 5 bits full part and 11 bits fractional
// and is supported only up to 32 Mhz
#define f_sf(f) ((sfreq_t)((f) >> 10))
#define sf_f(f) ((freq_t)(f) << 10)

#define KHZ_sf(k) f_sf(KHZ_f(k / 1000, k % 1000))

uint8_t* f2str(freq_t f, uint8_t *buffer,  uint8_t len);

#endif /* _FREQ_H_ */
