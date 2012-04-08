#ifndef __DDS_dds_h__
#define __DDS_dds_h__

#include <stdint.h>

#define DDS_PORT PORTB
#define DDS_DDR DDRB
#define DDS_CS 4

extern const uint32_t dds_f;

void dds_write(uint8_t cmd, uint8_t data);
void dds_init(void);
void dds_f1(uint32_t f);
void dds_f2(uint32_t f);

#endif
