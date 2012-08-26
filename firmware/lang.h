#ifndef LANG_DDS_02012012_MarSik
#define LANG_DDS_02012012_MarSik

#include <stdint.h>

#ifndef TEST
#include <avr/eeprom.h>
#endif

#ifndef EEMEM
#define EEMEM
#endif

/*
 * Language definitions
 */

extern const uint8_t s_title[] EEMEM;
extern const uint8_t s_author[] EEMEM;

extern const uint8_t s_initializing[] EEMEM;
extern const uint8_t s_i2c_error[] EEMEM;
extern const uint8_t s_si570_error[] EEMEM;

extern const uint8_t s_hex_prefix[] EEMEM;
extern const uint8_t s_cw[] EEMEM;
extern const uint8_t s_ssb[] EEMEM;
extern const uint8_t s_digi[] EEMEM;

extern const uint8_t s_ircode[] EEMEM;

/*
 * Band extra strings
 */
extern const uint8_t s_saq[]  EEMEM;
extern const uint8_t s_dcf77[]  EEMEM;
extern const uint8_t s_emergency[]  EEMEM;
extern const uint8_t s_image[]  EEMEM;

#endif /* LANG_DDS_02012012_MarSik */
