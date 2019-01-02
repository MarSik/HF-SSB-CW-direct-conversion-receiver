#ifndef __DDS_tuner_h__
#define __DDS_tuner_h__

#include <stdint.h>
#include "spi.h"
#include "freq.h"

typedef struct {
    sfreq_t freq;
    uint8_t cin;
    uint8_t l;
    uint8_t cout;
} tuning_record;

void tuner_save(void);
void tuner_set_from_record(tuning_record* record);
void tuner_record(tuning_record* record, freq_t freq);
void tuner_store(tuning_record* record);
uint8_t tuner_find(freq_t freq, tuning_record* record);

void tuner_write(void);
void tuner_init(void);

void tuner_up(uint8_t bank);
void tuner_down(uint8_t bank);

typedef enum {
 SYMETRIC = 0,
 ASYMETRIC = 1,
 ENDMODE = 2
} antenna_mode;

void tuner_mode(antenna_mode mode);
antenna_mode tuner_get_mode(void);

inline void tuner_next_mode(void) {
    tuner_mode((tuner_get_mode() + 1) % ENDMODE);
}

inline void tuner_prev_mode(void) {
    tuner_mode((tuner_get_mode() - 1) % ENDMODE);
}

#define BANK_CIN 0
#define BANK_L 1
#define BANK_COUT 2

uint8_t tuner_get_cin(void);
uint8_t tuner_get_cout(void);
uint8_t tuner_get_l(void);

uint16_t tuner_get_real_l(void);
uint16_t tuner_get_real_cout(void);
uint16_t tuner_get_real_cin(void);

extern const char* PICO;

#endif
