#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#include "drivers/gpio/gpio_types.h"
#include <stdint.h>

void gpio_init(gpio_conf_t *conf);
uint8_t gpio_read_level(gpio_t gpio);
void gpio_set_level(gpio_t gpio, uint8_t level);
void gpio_deinit(gpio_t gpio);

#endif
