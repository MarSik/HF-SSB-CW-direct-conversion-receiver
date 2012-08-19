#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

int i2c_read(uint8_t slave, uint8_t count, uint8_t *data);
int i2c_write(uint8_t slave, uint8_t count, uint8_t *data);

#endif /* _I2C_H_ */
