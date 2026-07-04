#include "i2c/i2c.h"
#include "i2c_bus.h"
#include "sync/mutex.h"
#include "task/task.h"

static mutex_t g_i2c_mutex[MAX_I2C_PERIPHERALS];

void hal_i2c_bus_init(i2c_perip_t perip)
{
    rtos_mutex_init(&g_i2c_mutex[perip]);
}

void hal_i2c_bus_lock(i2c_perip_t perip)
{
    rtos_mutex_lock(&g_i2c_mutex[perip], MAX_TIMEOUT);
}

void hal_i2c_bus_unlock(i2c_perip_t perip)
{
    rtos_mutex_unlock(&g_i2c_mutex[perip]);
}
