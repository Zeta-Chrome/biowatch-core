#ifndef DRIVER_SPI_BUS_H
#define DRIVER_SPI_BUS_H

#include "drivers/spi/spi.h"

void spi_bus_init(enum spi_perip perip);
void spi_bus_lock(enum spi_perip perip);
void spi_bus_unlock(enum spi_perip perip);

#endif
