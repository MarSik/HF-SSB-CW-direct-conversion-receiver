#ifndef __DDS_radio_h_MS__
#define __DDS_radio_h_MS__

#define F_MAX (15e6*10)
#define F_MIN (1e4*10)

#define F_DIR_UP 1
#define F_DIR_DOWN -1
#define OFFSET_DIR 1

#define FILTER_PORT PORTD
#define FILTER_DDR DDRD
#define FILTER_CW 5
#define FILTER_SSB 4

extern volatile uint32_t f;
extern volatile uint32_t f_step;

void set_cw(void);
void set_ssb(void);
void freq_offset(char dir);

void inline freq_step(uint16_t step)
{
    f_step = step;
}

void radio_init(void);

#endif
