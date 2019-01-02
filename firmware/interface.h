#ifndef __MT_interface__ms_20120326__
#define __MT_interface__ms_20120326__

#include <avr/io.h>
#include "config.h"

#define BUTTON_MASK (_BV(BUTTON_1) | _BV(BUTTON_2) | _BV(BUTTON_3) | _BV(BUTTON_4))

inline void led_on(uint8_t led)
{
    LED_PORT &= ~_BV(led);
}

inline void led_off(uint8_t led)
{
    LED_PORT |= _BV(led);
}

void interface_init(void);

typedef enum {
    INTF_FREQ = 0,
    INTF_TUNER_C,
    INTF_TUNER_L,
    INTF_TUNER_MODE,
    INTF_STEP
} intf_mode_t;

extern volatile intf_mode_t interface_mode;

void inline interface_mode_set(intf_mode_t m) {
    interface_mode = m;
}

extern uint8_t buffer[];
void interface_render(void);

#endif
