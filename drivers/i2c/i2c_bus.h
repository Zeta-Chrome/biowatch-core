#ifndef DRIVER_I2C_BUS_H
#define DRIVER_I2C_BUS_H

#include "drivers/i2c/i2c.h"

void i2c_bus_init(enum i2c_perip perip);
void i2c_bus_lock(enum i2c_perip perip);
void i2c_bus_unlock(enum i2c_perip perip);
void i2c_bus_reset(struct i2c_handle *handle);
void i2c_bus_reset_dma(struct i2c_handle *handle);

#endif
