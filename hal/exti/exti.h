#ifndef HAL_EXTI_H
#define HAL_EXTI_H

#include "hal/gpio/gpio_types.h"
#include "stm32wb55xx.h"

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
    uint8_t im;
    exti_edge_t edge;
    IRQn_Type irq; 
    uint8_t irq_priority; // 0-15
    exti_callback_t on_interrupt; 
} exti_conf_t;

void hal_exti_init(exti_conf_t *conf);
void hal_exti_gpio_init(exti_conf_t *conf);
void hal_exti_isr(uint8_t im);
void hal_exti_deinit(uint8_t im);
void hal_exti_gpio_deinit(gpio_t gpio);

#endif
