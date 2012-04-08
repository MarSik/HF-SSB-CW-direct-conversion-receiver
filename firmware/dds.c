#include <avr/io.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "spi.h"
#include "dds.h"

const uint32_t dds_f = 50e6 * 10;

static void dds_begin(void)
{
    DDS_PORT &= ~_BV(DDS_CS);
}

static void dds_end(void)
{
    DDS_PORT |= _BV(DDS_CS);
}

void dds_write(uint8_t cmd, uint8_t data)
{
    dds_begin();
    spi_transfer(cmd);
    spi_transfer(data);
    dds_end();
}

void dds_init(void)
{
    DDS_DDR |= _BV(DDS_CS);
    DDS_PORT |= _BV(DDS_CS); // CS idle high

    //0xF800 // Reset
    dds_write(0xf8, 0x00);

    //0x303D 0x210A 0x32D7 0x2323 // Freq0
    dds_write(0x30, 0x00);
    dds_write(0x21, 0x00);
    dds_write(0x32, 0x00);
    dds_write(0x23, 0x00);

    //0x1800 0x0900 // Phase0
    dds_write(0x18, 0x00);
    dds_write(0x09, 0x00);

    //0xA000 // Sync/Selsrc
    dds_write(0x90, 0x00);

    //0xC000 // Out of reset
    dds_write(0xc0, 0x00);
}

static uint32_t dds_convf(uint32_t f)
{
    return round(0xffffffff*(double)f/dds_f);
}

void dds_f1(uint32_t f)
{
    f = dds_convf(f);

    dds_write(0x30, (dds >> 0) & 0xff);
    dds_write(0x21, (dds >> 8) & 0xff);
    dds_write(0x32, (dds >> 16) & 0xff);
    dds_write(0x23, (dds >> 24) & 0xff);
}

void dds_f2(uint32_t f)
{
    f = dds_convf(f);

    dds_write(0x34, (dds >> 0) & 0xff);
    dds_write(0x25, (dds >> 8) & 0xff);
    dds_write(0x36, (dds >> 16) & 0xff);
    dds_write(0x27, (dds >> 24) & 0xff);
}
