#ifndef DRIVER_SPI_BUS_H
#define DRIVER_SPI_BUS_H

#include "drivers/spi/spi.h"

void spi_bus_init(spi_perip_t perip);
void spi_bus_lock(spi_perip_t perip);
void spi_bus_unlock(spi_perip_t perip);

#endif
