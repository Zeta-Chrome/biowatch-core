#include "drivers/i2c/i2c.h"
#include "i2c_bus.h"
#include "kernel/sync/mutex.h"
#include "kernel/task/task.h"

static struct mutex g_i2c_mutex[MAX_I2C_PERIPHERALS];

void i2c_bus_init(enum i2c_perip perip)
{
	kernel_mutex_init(&g_i2c_mutex[perip]);
}

void i2c_bus_lock(enum i2c_perip perip)
{
	kernel_mutex_lock(&g_i2c_mutex[perip], MAX_TIMEOUT);
}

void i2c_bus_unlock(enum i2c_perip perip)
{
	kernel_mutex_unlock(&g_i2c_mutex[perip]);
}

void i2c_bus_reset(struct i2c_handle *handle)
{
	kernel_mutex_lock(&g_i2c_mutex[handle->perip], MAX_TIMEOUT);
	i2c_reset(handle);
	kernel_mutex_unlock(&g_i2c_mutex[handle->perip]);
}

void i2c_bus_reset_dma(struct i2c_handle *handle)
{
	kernel_mutex_lock(&g_i2c_mutex[handle->perip], MAX_TIMEOUT);
	i2c_reset(handle);
	kernel_mutex_unlock(&g_i2c_mutex[handle->perip]);
}
