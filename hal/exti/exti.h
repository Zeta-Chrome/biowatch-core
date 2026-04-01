#ifndef HAL_EXTI_H
#define HAL_EXTI_H

#include "hal/gpio/gpio_types.h"

typedef enum 
{
    EXTI_EDGE_RISING = 0x1,
    EXTI_EDGE_FALLING = 0x2,
    EXTI_EDGE_BOTH = 0x3,
} exti_edge_t;

typedef void (*exti_callback_t)(void);

typedef struct 
{
    gpio_t gpio;
    exti_edge_t edge;
    exti_callback_t on_interrupt; 
} exti_conf_t;

void hal_exti_init(exti_conf_t *conf);
void hal_exti_isr(uint8_t line);
void hal_exti_deinit(gpio_t gpio);

#endif
