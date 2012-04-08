#ifndef __DDS_radio_h_MS__
#define __DDS_radio_h_MS__

/* All frequency methods accept frequency multiplied by 10,
   so we can represent fractions with one decimal precision */

#define F_MAX (15e6*10)
#define F_MIN (1e4*10)

#define F_DIR_UP 1
#define F_DIR_DOWN -1
#define OFFSET_DIR 1

#define STEP_MAX 1e6
#define STEP_MIN 10

#define FILTER_PORT PORTD
#define FILTER_DDR DDRD
#define FILTER_CW 5
#define FILTER_SSB 4

/* frequency is stored multiplied by 10
   so we can use fixed point with one decimal */
extern volatile uint32_t f;
extern volatile uint32_t f_step;

void radio_init(void);

void set_cw(void);
void set_ssb(void);
void freq_step(char dir);

void step_up(void);
void step_down(void);

void inline step_set(uint16_t step)
{
    f_step = step;
}

/* set F to frequency fr/10 */
void inline freq_set(uint32_t fr)
{
    f = fr;
}

#endif
