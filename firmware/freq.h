#ifndef _FREQ_H_
#define _FREQ_H_

#include <stdint.h>

/* All frequency methods accept frequency in fixed point with 1 fractional bit,
   so we can represent fractions with half a hertz precision */
typedef uint32_t freq_t;
typedef uint16_t sfreq_t;

#define MHZ_f(m) (((freq_t)(m) * 1000000) << 1)
#define KHZ_f(m, k) (MHZ_f(m) + (((freq_t)(k) * 1000) << 1))
#define HZ_f(m, k, h) (KHZ_f(m, k) + (((freq_t)(h)) << 1))
#define FHZ_f(m, k, h, f) (HZ_f(m, k, h) + ((f / 1000) << 1) + ((f % 1000 >= 500) ? 1 : 0))

#define f_full(f) (((freq_t)(f)) >> 1)
#define f_frac(f) ((f) & 0b1)

#define f_MHZ(f) (f_full(f) / 1000000)
#define f_KHZ(f) ((f_full(f) % 1000000) / 1000)
#define f_HZ(f) (f_full(f) % 1000)

// short frequency is the number of kilohertz
// and is supported only up to 32 Mhz
inline sfreq_t f_sf(freq_t f) { return ((sfreq_t)((f) / (1000 << 1))); }
inline freq_t sf_f(sfreq_t f) { return ((freq_t)(f) * (1000 << 1)); }

#define KHZ_sf(k) (k)

uint8_t* f2str(freq_t f, uint8_t *buffer,  uint8_t len);

#endif /* _FREQ_H_ */
