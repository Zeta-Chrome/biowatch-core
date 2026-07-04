#ifndef HAL_I2C_BUS_H
#define HAL_I2C_BUS_H

#include "hal/i2c/i2c.h"

void hal_i2c_bus_init(i2c_perip_t perip);
void hal_i2c_bus_lock(i2c_perip_t perip);
void hal_i2c_bus_unlock(i2c_perip_t perip);

#endif
