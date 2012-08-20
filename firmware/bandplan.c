#include <stdint.h>
#include <avr/eeprom.h>
#include "bandplan.h"
#include "lang.h"
#include "freq.h"

#define BANDPLAN_LEN 44
#define BANDPLAN_ENTRY_LEN 3
#define BAND(from, to, flags, extra) KHZ_sf(from), KHZ_sf(to), ((flags) << 8) + (extra)

enum {
    EX_NOTHING = 0,
    EX_SAQ,
    EX_DCF77,
    EX_EMERGENCY,
    EX_IMAGE,
};

const uint8_t* bandplan_extra[] EEMEM = {
    [EX_SAQ] = s_saq,
    [EX_DCF77] = s_dcf77,
    [EX_EMERGENCY] = s_emergency,
    [EX_IMAGE] = s_image,
};


/* table
   2B - starting kHz, uint16_t
   2B - ending kHz, uint16_t
   1B - flags
   1B - extra str, uint8_t
*/


uint16_t bandplan_table[BANDPLAN_LEN * BANDPLAN_ENTRY_LEN] EEMEM = {
    BAND(12, 12, 0, EX_SAQ),
    BAND(77, 77, 0, EX_DCF77),

    BAND(1810, 1837, CW, 0),
    BAND(1838, 1839, CW | DIGI | NARROW, 0),
    BAND(1840, 1842, DIGI, 0),
    BAND(1843, 1999, ALLM, 0),

    BAND(3500, 3509, CW | DX, 0),
    BAND(3510, 3559, CW, 0),
    BAND(3560, 3560, CW | QRP, 0),
    BAND(3561, 3579, CW, 0),
    BAND(3580, 3589, DIGI | NARROW, 0),
    BAND(3590, 3599, DIGI | NARROW | UNATTENDED, 0),
    BAND(3600, 3619, DIGI, 0),
    BAND(3620, 3689, ALLM, 0),
    BAND(3690, 3690, SSB | QRP, 0),
    BAND(3691, 3774, ALLM, 0),
    BAND(3775, 3800, ALLM | DX, 0),

    BAND(7000, 7029, CW, 0),
    BAND(7030, 7030, CW | QRP, 0),
    BAND(7031, 7039, CW, 0),
    BAND(7040, 7046, NARROW | DIGI, 0),
    BAND(7047, 7049, NARROW | DIGI | UNATTENDED, 0),
    BAND(7050, 7052, ALLM | UNATTENDED, 0),
    BAND(7053, 7059, DIGI, 0),
    BAND(7060, 7089, ALLM, 0),
    BAND(7090, 7090, SSB | QRP, 0),
    BAND(7091, 7174, ALLM, 0),
    BAND(7175, 7200, ALLM | DX, 0),

    BAND(10100, 10115, CW, 0),
    BAND(10116, 10116, CW | QRP, 0),
    BAND(10117, 10139, CW, 0),
    BAND(10140, 10149, DIGI | NARROW, 0),

    BAND(14000, 14059, CW, 0),
    BAND(14060, 14060, CW | QRP, 0),
    BAND(14061, 14069, CW, 0),
    BAND(14070, 14088, NARROW | DIGI, 0),
    BAND(14089, 14098, NARROW | DIGI | UNATTENDED, 0),
    BAND(14099, 14100, UNATTENDED, 0),
    BAND(14101, 14111, DIGI | UNATTENDED, 0),
    BAND(14112, 14189, ALLM, 0),
    BAND(14190, 14199, ALLM | DX, 0),
    BAND(14200, 14284, ALLM, 0),
    BAND(14285, 14285, SSB | QRP, 0),
    BAND(14286, 14349, ALLM, 0)
};


/* search for band in sorted array bandplan_table */
uint8_t bandplan(sfreq_t f, const uint8_t** desc)
{
    uint8_t idxa = 0;
    uint8_t idxb = BANDPLAN_LEN - 1;

    do {
        uint8_t idx = (idxa + idxb) / 2;
        uint16_t fr = eeprom_read_word(bandplan_table + BANDPLAN_ENTRY_LEN * idx);
        uint16_t to = eeprom_read_word(bandplan_table + BANDPLAN_ENTRY_LEN * idx + 1);

        /* we found the band! */
        if (fr <= f && to >= f) {
            uint16_t ex = eeprom_read_word(bandplan_table + BANDPLAN_ENTRY_LEN * idx + 2);
            uint8_t f = ex >> 8;

            ex &= 0xff;
            if (ex) *desc = (uint8_t*)eeprom_read_word((uint16_t*)bandplan_extra + ex);
            else *desc = NULL;

            return f;
        }

        /* we are looking too high */
        else if (fr > f) {
            // we are reached the lower end of the table and found nothing
            if (idx == 0) {
                *desc = NULL;
                return 0;
            }

            idxb = idx - 1;
        }

        /* we are looking too low */
        else if (to < f) {
            idxa = idx + 1;

            // we are reached the upper side of the table and found nothing
            if (idx == BANDPLAN_LEN) {
                *desc = NULL;
                return 0;
            }
        }

        /* idxa > idxb means no such band was found */
    } while(idxa <= idxb);

    *desc = NULL;
    return 0;
}
