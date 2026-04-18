#include "exti.h"
#include "gpio/gpio.h"
#include "utils/assert.h"
#include "stm32wb55xx.h"
#include "reg.h"

#define NULL ((void *)0)
#define MAX_EXTI_LINES 50

static IRQn_Type g_exti_irq[MAX_EXTI_LINES];
static exti_callback_t g_exti_callbacks[MAX_EXTI_LINES];

uint8_t gpio_port_index(GPIO_TypeDef *port);

void hal_exti_init(exti_conf_t *conf)
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

    g_exti_callbacks[conf->im] = conf->on_interrupt;
    g_exti_irq[conf->im] = conf->irq;

    // Enable interrupt
    NVIC_EnableIRQ(conf->irq);
    NVIC_SetPriority(conf->irq, conf->irq_priority);
}

void hal_exti_gpio_init(exti_conf_t *conf)
{
    // Configure as input pin
    gpio_conf_t int_conf = gpio_conf_input(conf->gpio, GPIO_PULL_NONE);
    hal_gpio_init(&int_conf);

    uint8_t port = gpio_port_index(conf->gpio.port);
    uint8_t pin = conf->gpio.pin;

    // Set the port in the EXTICR
    reg_set_field(&SYSCFG->EXTICR[pin / 4], (pin % 4) * 4, 3, port);

    hal_exti_init(conf);
}

void hal_exti_isr(uint8_t im)
{
    // Clear pending bit
    EXTI->PR1 = (1U << im);

    if (g_exti_callbacks[im] != NULL)
        g_exti_callbacks[im]();
}

void hal_exti_deinit(uint8_t im)
{
    // Mask the interrupt line
    reg_clear_bit(&EXTI->IMR1, im);

    // Disable edge triggers
    reg_clear_bit(&EXTI->FTSR1, im);
    reg_clear_bit(&EXTI->RTSR1, im);

    // Clear any pending interrupt
    reg_set_bit(&EXTI->PR1, im);

    // Clear callback
    g_exti_callbacks[im] = NULL;

    // Disable interrupt
    NVIC_DisableIRQ(g_exti_irq[im]);
}

void hal_exti_gpio_deinit(gpio_t gpio)
{
    uint8_t pin = gpio.pin;

    // Clear SYSCFG EXTICR
    reg_set_field(&SYSCFG->EXTICR[pin / 4], (pin % 4) * 4, 3, 0);

    hal_exti_deinit(pin);
    hal_gpio_deinit(gpio);
}
