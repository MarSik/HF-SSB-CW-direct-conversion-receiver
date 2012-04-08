#ifndef __MS_DDS_memory_h__
#define __MS_DDS_memory_h__

#include <stdint.h>

void memory_save(uint8_t no, uint16_t kHz, uint16_t Hz, uint8_t flags);
void memory_load(uint8_t no, uint16_t *kHz, uint16_t *Hz, uint8_t *flags);
void memory_name(uint8_t no, const uint8_t* name); 

#endif
