#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

void i2c_init(void);
uint8_t i2c_status(void);

#define I2C_READ(a) (((a) << 1) + 1)
#define I2C_WRITE(a) (((a) << 1) + 0)

typedef void (*i2c_callback)(void*, uint8_t);

int i2c_transfer(const uint8_t slave, const uint8_t start_reg,
                 uint8_t *buffer, const uint8_t data_count,
                 const i2c_callback cb, void *cb_data);

#endif /* _I2C_H_ */
