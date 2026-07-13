#include "kernel/sync/mutex.h"
#include "kernel/task/task.h"
#include "spi.h"
#include "spi_bus.h"

static mutex_t g_spi_mutex[MAX_SPI_PERIPHERALS];

void spi_bus_init(spi_perip_t perip)
{
    kernel_mutex_init(&g_spi_mutex[perip]);
}

void spi_bus_lock(spi_perip_t perip)
{
    kernel_mutex_lock(&g_spi_mutex[perip], MAX_TIMEOUT);
}

void spi_bus_unlock(spi_perip_t perip)
{
    kernel_mutex_unlock(&g_spi_mutex[perip]);
}
