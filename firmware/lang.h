#ifndef LANG_DDS_02012012_MarSik
#define LANG_DDS_02012012_MarSik

#include <stdint.h>

#ifndef TEST
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#endif

#ifndef EEMEM
#define EEMEM
#endif

#ifndef PROGMEM
#define PROGMEM
#endif


/*
 * Language definitions
 */

extern const uint8_t s_title[] PROGMEM;
extern const uint8_t s_author[] PROGMEM;

extern const uint8_t s_initializing[] PROGMEM;
extern const uint8_t s_i2c_error[] PROGMEM;
extern const uint8_t s_si570_error[] PROGMEM;

extern const uint8_t s_hex_prefix[] PROGMEM;
extern const uint8_t s_cw[] PROGMEM;
extern const uint8_t s_ssb[] PROGMEM;
extern const uint8_t s_digi[] PROGMEM;

extern const uint8_t s_ircode[] PROGMEM;

/*
 * Band extra strings
 */
extern const uint8_t s_saq[]  PROGMEM;
extern const uint8_t s_dcf77[]  PROGMEM;
extern const uint8_t s_emergency[]  PROGMEM;
extern const uint8_t s_image[]  PROGMEM;

#endif /* LANG_DDS_02012012_MarSik */
