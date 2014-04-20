#ifndef MS_TUNER_CONFIG_H_1404
#define MS_TUNER_CONFIG_H_1404

/* Serial lines
   UART0 is shared with bootloader and should use
   the same settings = 38400 8n1
 */
#define BAUD_RATE0 38400
#define BAUD_RATE1 38400

/* Tuner box configuration
 */
#define BANK1 BANK_COUT
#define BANK2 BANK_CIN
#define BANK3 BANK_L

#define BANK1_INVERT 0
#define BANK2_INVERT 0
#define BANK3_INVERT 1

/* SPI configuration
*/
#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_MISO 6
#define SPI_MOSI 5
#define SPI_SCK 7
#define SPI_SS 4
#define SPI_SS2 3

/* Configure buttons
 */
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

//#define KEY_ENABLE
#define KEY_PORT PORTD
#define KEY_PIN PIND
#define KEY_DDR DDRD
#define KEY_A 0 // RX
#define KEY_B 1 // TX

#define LED_DDR  DDRA
#define LED_PORT PORTA
#define LED_PIN  PINA
#define LEDA     PINA7
#define LEDB     PINA6

/*
 * LCD configuration
 *
 * all data & control lines have to be connected to
 * one port (PORTX) on the mcu
 */

#define LCD PORTA
#define LCD_PIN PINA
#define LCD_DDR DDRA

/* LCD data lines are connected to
 *        D4 -- PORTX0
 *        D5 -- PORTX1
 *        D6 -- PORTX2
 *        D7 -- PORTX3
 *
 * control lines are connected to Px (defined below)
 *  (RW is tied to GND)
 */
#define RS 5
#define EN 4


#endif
