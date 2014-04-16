#ifndef __MT_interface__ms_20120326__
#define __MT_interface__ms_20120326__

#include <avr/io.h>

#define ROTARY_PORT PORTD
#define ROTARY_PIN PIND
#define ROTARY_DDR DDRD
#define ROTARY_BUTTON 5
#define ROTARY_A 6
#define ROTARY_B 7
#define ROTARY_SHIFT 6 // how much bits must be shifted left to get AB on the two LSB (mask 0b11)
#define ROTARY_DIR 1 //1 or -1 if need to reverse rotation arises

#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_DDR DDRC
#define BUTTON_1 3
#define BUTTON_2 4
#define BUTTON_3 5
#define BUTTON_4 6
#define BUTTON_MASK (_BV(BUTTON_1) | _BV(BUTTON_2) | _BV(BUTTON_3) | _BV(BUTTON_4))

#define KEY_PORT PORTD
#define KEY_PIN PIND
#define KEY_DDR DDRD
#define KEY_A 0 // RX
#define KEY_B 1 // TX

void interface_init(void);

typedef enum {
    INTF_FREQ = 0,
    INTF_TUNER_C,
    INTF_TUNER_L,
    INTF_STEP
} intf_mode_t;

extern volatile intf_mode_t interface_mode;

void inline interface_mode_set(intf_mode_t m) {
    interface_mode = m;
}

#endif
