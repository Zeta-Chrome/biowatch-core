#include "exti.h"
#include "gpio/gpio.h"
#include "utils/assert.h"
#include "stm32wb55xx.h"
#include "reg.h"

#define NULL ((void *)0)
#define MAX_EXTI_LINES 50

static exti_handle_t *g_exti_handles[MAX_EXTI_LINES];

uint8_t gpio_port_index(GPIO_TypeDef *port);

void hal_exti_init(exti_conf_t *conf, exti_handle_t *handle)
{
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Enable rising or falling edge
    if ((conf->edge & EXTI_EDGE_RISING) != 0)
    {
        reg_set_bit(&EXTI->RTSR1, conf->im);
    }

    if ((conf->edge & EXTI_EDGE_FALLING) != 0)
    {
        reg_set_bit(&EXTI->FTSR1, conf->im);
    }

    EXTI->PR1 = (1U << conf->im);

    // Unmask interrupt line
    reg_set_bit(&EXTI->IMR1, conf->im);

    handle->im = conf->im;
    handle->irq = conf->irq;
    handle->callback = conf->callback;
    handle->user_data = conf->user_data;
    g_exti_handles[conf->im] = handle;

    // Enable interrupt
    NVIC_EnableIRQ(conf->irq);
    NVIC_SetPriority(conf->irq, conf->irq_priority);
}

void hal_exti_gpio_init(exti_conf_t *conf, exti_handle_t *handle)
{
    // Configure as input pin
    gpio_conf_t int_conf = gpio_conf_input(conf->gpio, GPIO_PULL_NONE);
    hal_gpio_init(&int_conf);

    uint8_t port = gpio_port_index(conf->gpio.port);
    uint8_t pin = conf->gpio.pin;

    // Set the port in the EXTICR
    reg_set_field(&SYSCFG->EXTICR[pin / 4], (pin % 4) * 4, 3, port);

    hal_exti_init(conf, handle);
    handle->gpio = conf->gpio;
}

void hal_exti_isr(uint8_t im)
{
    // Clear pending bit
    EXTI->PR1 = (1U << im);

    if (g_exti_handles[im]->callback != NULL)
    {
        g_exti_handles[im]->callback(g_exti_handles[im]->user_data);
    }
}

void hal_exti_deinit(exti_handle_t *handle)
{
    // Mask the interrupt line
    reg_clear_bit(&EXTI->IMR1, handle->im);

    // Disable edge triggers
    reg_clear_bit(&EXTI->FTSR1, handle->im);
    reg_clear_bit(&EXTI->RTSR1, handle->im);

    // Clear any pending interrupt
    reg_set_bit(&EXTI->PR1, handle->im);

    // Clear callback
    g_exti_handles[handle->im]->callback = NULL;

    // Disable interrupt
    NVIC_DisableIRQ(g_exti_handles[handle->im]->irq);
}

void hal_exti_gpio_deinit(exti_handle_t *handle)
{
    uint8_t pin = handle->gpio.pin;

    // Clear SYSCFG EXTICR
    reg_set_field(&SYSCFG->EXTICR[pin / 4], (pin % 4) * 4, 3, 0);

    hal_exti_deinit(handle);
    hal_gpio_deinit(handle->gpio);
}
