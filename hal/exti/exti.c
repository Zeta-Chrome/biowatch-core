#include "hal/gpio/gpio.h"
#include "stm32wb55xx.h"
#include "hal/reg.h"
#include "hal/exti/exti.h"
#include "utils/assert.h"

#define NULL ((void *)0)
#define MAX_EXTI_LINES 16

static exti_callback_t g_exti_callbacks[MAX_EXTI_LINES];

uint8_t gpio_port_index(GPIO_TypeDef *port);

void hal_exti_init(exti_conf_t *conf)
{
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Configure as input pin
    gpio_conf_t int_conf = gpio_conf_input(conf->gpio, GPIO_PULL_NONE);  // Has external interrupt
    hal_gpio_init(&int_conf);

    // Enable interrupt
    NVIC_EnableIRQ(conf->irq);
    NVIC_SetPriority(conf->irq, conf->irq_priority);

    uint8_t port = gpio_port_index(conf->gpio.port);
    uint8_t pin = conf->gpio.pin;

    // Set the port in the EXTICR
    reg_set_field(&SYSCFG->EXTICR[pin / 4], (pin % 4) * 4, 3, port);

    // Enable rising or falling edge
    if ((conf->edge & EXTI_EDGE_RISING) != 0)
    {
        reg_set_bit(&EXTI->RTSR1, pin);
    }
    else if ((conf->edge & EXTI_EDGE_FALLING) != 0)
    {
        reg_set_bit(&EXTI->FTSR1, pin);
    }

    // Unmask interrupt line
    reg_set_bit(&EXTI->IMR1, pin);

    g_exti_callbacks[pin] = conf->on_interrupt;
}

void hal_exti_isr(uint8_t line)
{
    // Clear pending bit
    reg_set_bit(&EXTI->PR1, line);

    if (g_exti_callbacks[line] != NULL)
        g_exti_callbacks[line]();
}

void hal_exti_deinit(gpio_t gpio)
{
    uint8_t pin = gpio.pin;

    // Mask the interrupt line
    reg_clear_bit(&EXTI->IMR1, pin);

    // Disable edge triggers
    reg_clear_bit(&EXTI->FTSR1, pin);
    reg_clear_bit(&EXTI->RTSR1, pin);

    // Clear any pending interrupt
    reg_set_bit(&EXTI->PR1, pin);

    // Clear SYSCFG EXTICR
    reg_set_field(&SYSCFG->EXTICR[pin / 4], (pin % 4) * 4, 3, 0);

    // Clear callback
    g_exti_callbacks[pin] = NULL;
}
