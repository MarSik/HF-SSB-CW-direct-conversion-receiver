#include <stdint.h>
#include <avr/eeprom.h>
#include "bandplan.h"
#include "lang.h"

#define BANDPLAN_LEN 9
#define BANDPLAN_ENTRY_LEN 3
#define BAND(from, to, flags, extra) (from), (to), ((flags) << 8) + (extra)

/* table
   2B - starting kHz, uint16_t
   2B - ending kHz, uint16_t
   1B - flags
   1B - extra str, uint8_t
*/

uint16_t bandplan_table[BANDPLAN_LEN * BANDPLAN_ENTRY_LEN] EEMEM = {
    BAND(3500, 3509, TX | CW | DX, 0),
    BAND(3510, 3539, TX | CW, 0),
    BAND(3540, 3559, TX | CW | DIGI, 0),
    BAND(3560, 3580, TX | DIGI, 0),
    BAND(3600, 3800, TX | SSB, 0),
    BAND(7000, 7350, TX, 0),
    BAND(10000, 10140, TX | CW, 0),
    BAND(10140, 10150, TX | DIGI, 0),
    BAND(14000, 14999, TX, 0)
};

const uint8_t* bandplan_extra[] EEMEM = {
    s_sat_dl,
    s_sat_ul
};

/* search for band in sorted array bandplan_table */
uint8_t bandplan(uint16_t kHz, const uint8_t** desc)
{
    uint8_t idxa = 0;
    uint8_t idxb = BANDPLAN_LEN - 1;

    do {
        uint8_t idx = (idxa + idxb) / 2;
        uint16_t fr = eeprom_read_word(bandplan_table + 3 * idx);
        uint16_t to = eeprom_read_word(bandplan_table + 3 * idx + 1);

        /* we found the band! */
        if (fr <= kHz && to >= kHz) {
            uint16_t ex = eeprom_read_word(bandplan_table + 3 * idx + 2);
            uint8_t f = ex >> 8;

            ex &= 0xff;
            if (ex) *desc = bandplan_extra[ex - 1];
            else *desc = NULL;

            return f;
        }

        /* we are looking too high */
        else if (fr > kHz) {
            idxb = idx - 1;
        }

        /* we are looking too low */
        else if (to < kHz) {
            idxa = idx + 1;
        }

        /* idxa > idxb means no such band was found */
    } while(idxa <= idxb);

    return 0;
}
