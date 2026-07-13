#ifndef DRIVER_I2C_BUS_H
#define DRIVER_I2C_BUS_H

#include "drivers/i2c/i2c.h"

void i2c_bus_init(i2c_perip_t perip);
void i2c_bus_lock(i2c_perip_t perip);
void i2c_bus_unlock(i2c_perip_t perip);
void i2c_bus_reset(i2c_handle_t *handle);
void i2c_bus_reset_dma(i2c_handle_t *handle);

#endif
