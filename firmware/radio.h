#ifndef __DDS_radio_h_MS__
#define __DDS_radio_h_MS__

/* All frequency methods accept frequency in fixed point with 21 fractional bits,
   so we can represent fractions with half a hertz precision */

#include "freq.h"

#define F_MAX MHZ_f(30)
#define F_MIN KHZ_f(2, 500)

#define F_DIR_UP ((signed char)1)
#define F_DIR_DOWN ((signed char)-1)

#define OFFSET_DIR ((signed char)1)

#define STEP_MAX MHZ_f(1)
#define STEP_MIN HZ_f(0, 0, 10)

#define FILTER_PORT PORTD
#define FILTER_DDR DDRD
#define FILTER_CW 5
#define FILTER_SSB 4

/* frequency is stored multiplied by 10
   so we can use fixed point with one decimal */
extern volatile freq_t f;
extern volatile freq_t f_step;

void radio_init(void);

void set_cw(void);
void set_ssb(void);
void freq_step(signed char dir);

void step_up(void);
void step_down(void);

void inline step_set(freq_t step)
{
    f_step = step;
}

/* set F to frequency fr.khz [11b:21b] */
void inline freq_set(freq_t fr)
{
    f = fr;
}

#endif
