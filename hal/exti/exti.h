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

typedef void (*exti_callback_t)(void *user_data);

typedef struct
{
    union
    {
        // Both pin and im occupy the same space
        struct
        {
            gpio_t gpio; // 1 byte of pin + 8 bytes of GPIO_Typedef *
            gpio_pupd_t pupd;
        };
        uint8_t im;   // 1 byte of im
    };
    exti_edge_t edge;
    IRQn_Type irq;
    uint8_t irq_priority;  // 0-15
    exti_callback_t callback;
    void *user_data;
} exti_conf_t;

typedef struct
{
    union
    {
        struct
        {
            gpio_t gpio;
            gpio_pupd_t pupd;
        };

        uint8_t im;
    };
    IRQn_Type irq;
    exti_callback_t callback;
    void *user_data;
} exti_handle_t;

void hal_exti_init(exti_conf_t *conf, exti_handle_t *handle);
void hal_exti_gpio_init(exti_conf_t *conf, exti_handle_t *handle);
void hal_exti_isr(uint8_t im);
void hal_exti_deinit(exti_handle_t *handle);
void hal_exti_gpio_deinit(exti_handle_t *handle);

#endif
