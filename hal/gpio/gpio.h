#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "hal/gpio/gpio_types.h"

void hal_gpio_init(gpio_conf_t *conf);
uint8_t hal_gpio_read_level(gpio_t gpio);
void hal_gpio_set_level(gpio_t gpio, uint8_t level);
void hal_gpio_deinit(gpio_t gpio);

#endif
