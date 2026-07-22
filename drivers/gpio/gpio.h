#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#include "drivers/gpio/gpio_types.h"
#include <stdint.h>

void gpio_init(struct gpio_conf *conf);
uint8_t gpio_read_level(struct gpio gpio);
void gpio_set_level(struct gpio gpio, uint8_t level);
void gpio_deinit(struct gpio gpio);

#endif
