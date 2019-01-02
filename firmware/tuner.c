#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "config.h"
#include "spi.h"
#include "tuner.h"
#include "interface.h"
#include "freq.h"
#include "radio.h"

volatile static uint8_t banks[3] = {0x00, 0x00, 0x00};
volatile static antenna_mode mode = SYMETRIC;

// Cout is in 10x pF (1 == 0.1pF)
const static uint16_t cout_values[8] = {37, 75, 147, 303, 576, 1200, 2319, 4807};

// L is in nH (1 == 1nH, 1000 = 1uH)
const static uint16_t l_values[8] = {64, 144, 256, 484, 1024, 2073, 3953, 7938};
const static uint16_t cin_values[8] = {0, 0, 0, 0, 0, 0, 0, 0};

const char* PICO = "pnum kM";

// Table for tuning records
tuning_record tuner_records[256] EEMEM;

void tuner_write_raw(uint8_t b1, uint8_t b2, uint8_t b3)
{
    led_on(LEDA);
    spi_begin();
    /* Driver is inverting signal so non-inverted means we have to
       send the inverted value. Inverted means we have to send the
       normal value */
    spi_transfer((BANK3_INVERT) ? b3 : ~b3);
    spi_transfer((BANK2_INVERT) ? b2 : ~b2);
    spi_transfer((BANK1_INVERT) ? b1 : ~b1);
    spi_end();
    led_off(LEDA);
}

void tuner_write(void)
{
    uint8_t bank1_mask, bank2_mask, bank3_mask;

    if (mode == ASYMETRIC) {
        bank1_mask = BANK1_ASY_MASK;
        bank2_mask = BANK2_ASY_MASK;
        bank3_mask = BANK3_ASY_MASK;
    } else {
        bank1_mask = BANK1_SYM_MASK;
        bank2_mask = BANK2_SYM_MASK;
        bank3_mask = BANK3_SYM_MASK;
    }

    tuner_write_raw(banks[BANK1] & bank1_mask,
                    banks[BANK2] & bank2_mask,
                    banks[BANK3] & bank3_mask);
}

void tuner_save(void)
{
    tuning_record record;
    tuner_record(&record, f);
    tuner_store(&record);
}

void tuner_up(uint8_t bank)
{
    if (banks[bank] < 0xff) ++banks[bank];
}

void tuner_down(uint8_t bank)
{
    if (banks[bank] > 0) --banks[bank];
}

void tuner_mode(antenna_mode new_mode) {
    mode = new_mode;
}

antenna_mode tuner_get_mode() {
    return mode;
}

void tuner_init(void)
{
    // Initialize relay driver capacitors to known state
    // Set all relays to 0
    banks[2] = banks[1] = banks[0] = 0;
    tuner_write();

    for (uint8_t i = 0; i<3*8; i++) {

        _delay_ms(250);

        // Set 1 for a single relay
        banks[(i & 0x18) >> 3] = 1 << (i & 0x7);
        tuner_write_raw(banks[0], banks[1], banks[2]);

        _delay_ms(50);

        // Set all relays back to 0
        banks[2] = banks[1] = banks[0] = 0;
        tuner_write();
    }
}

uint8_t tuner_get_cin(void)
{
    return banks[BANK_CIN];
}

uint8_t tuner_get_cout(void)
{
    return banks[BANK_COUT];
}

uint8_t tuner_get_l(void)
{
    return banks[BANK_L];
}

inline uint16_t compute_bits(uint8_t val, const uint16_t table[])
{
    uint8_t index = 0;
    uint16_t ret = 0;

    while(val) {
        if (val & 0x1) ret += table[index];
        index++;
        val >>= 1;
    }

    return ret;
}

uint16_t tuner_get_real_l(void)
{
    return compute_bits(tuner_get_l(), l_values);
}

uint16_t tuner_get_real_cout(void)
{
    return compute_bits(tuner_get_cout(), cout_values) / 10;
}

uint16_t tuner_get_real_cin(void)
{
    return compute_bits(tuner_get_cin(), cin_values) / 10;
}

inline sfreq_t sf_rec(sfreq_t freq)
{
    return freq & 0xfff8;
}

inline uint8_t sf_hash(sfreq_t freq)
{
    // Compute hash id - strip low 3 bits and use
    // the lowest 8 remaining bits as index
    return (freq >> 3) & 0xff;
}

void tuner_set_from_record(tuning_record* record)
{
    banks[BANK_L] = record->l;
    banks[BANK_CIN] = record->cin;
    banks[BANK_COUT] = record->cout;
}

void tuner_record(tuning_record* record, freq_t freq)
{
    record->l = banks[BANK_L];
    record->cin = banks[BANK_CIN];
    record->cout = banks[BANK_COUT];
    record->freq = sf_rec(f_sf(freq));
}

const uint8_t SKIP_REMOVED=1;
const uint8_t RETURN_REMOVED=0;

uint8_t* tuner_find_slot(sfreq_t freq, sfreq_t* freqslot, uint8_t skip_removed) {
    // Use open hash addressing
    // That is start at the hash of freq
    // and find the first slot with 0
    // or the freq
    // set the freq of selected slot to freqslot
    // and return the eeprom address
    uint16_t i = sf_hash(freq);
    tuning_record *idx;
    do {
        idx = tuner_records + i;
        // check if slot matches or empty (freq == 0x00)
        // return deleted slot (Oxffff) only if skip_removed is 0
        *freqslot = sf_rec(eeprom_read_word((uint16_t*)idx));
        if (*freqslot == sf_rec(freq) || *freqslot == 0x0000
            || (!skip_removed && *freqslot == 0xffff)) break;
        ++i;
    } while(i != sf_hash(freq));

    return idx;
}

void tuner_store(tuning_record* record)
{
    sfreq_t slot;
    uint8_t *addr = tuner_find_slot(record->freq, &slot, RETURN_REMOVED);
    if (slot == 0x0000 || slot == 0xffff) eeprom_write_word(addr, record->freq);
    else if (slot != record->freq) {
        /* XXX the memory is full! */
        led_on(LEDA);
        return;
    }

    eeprom_write_byte(addr+2, record->cin);
    eeprom_write_byte(addr+3, record->cout);
    eeprom_write_byte(addr+4, record->l);
}

uint8_t tuner_find(freq_t freq, tuning_record* record)
{
    sfreq_t slot;
    uint8_t *addr = tuner_find_slot(f_sf(freq), &slot, SKIP_REMOVED);
    if (slot == sf_rec(f_sf(freq))) {
        record->freq = slot;
        record->cin = eeprom_read_byte(addr+2);
        record->cout = eeprom_read_byte(addr+3);
        record->l = eeprom_read_byte(addr+4);
        return 1;
    }
    else if (slot == 0x0000 || slot == 0xffff) {
        return 0;
    }
    else {
        led_on(LEDA);
        return 0;
    }
}
