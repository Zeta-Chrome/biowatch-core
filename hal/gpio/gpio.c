#include "hal/gpio/gpio.h"
#include "hal/reg.h"
#include "utils/assert.h"

#ifdef DEBUG
// one bit per pin, one entry per port
static uint16_t g_reserved_pins[5] = {0};  // A, B, C, D, E

uint8_t gpio_port_index(GPIO_TypeDef *port)
{
    return ((uint8_t *)port - (uint8_t *)GPIOA) / ((uint8_t *)GPIOB - (uint8_t *)GPIOA);
}

static void gpio_reserve_pins(gpio_t gpio)
{
    uint8_t idx = gpio_port_index(gpio.port);
    uint16_t mask = (1U << gpio.pin);

    BW_ASSERT(!(g_reserved_pins[idx] & mask), "GPIO CONFLICT: Pin %d.%d already reserved", gpio.pin, idx);

    g_reserved_pins[idx] |= mask;
}
#endif

static void gpio_clock_init(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        reg_set_bit(&RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN_Pos);
    }
    else if (port == GPIOB)
    {
        reg_set_bit(&RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN_Pos);
    }
    else if (port == GPIOC)
    {
        reg_set_bit(&RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN_Pos);
    }
    else if (port == GPIOD)
    {
        reg_set_bit(&RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN_Pos);
    }
    else if (port == GPIOE)
    {
        reg_set_bit(&RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN_Pos);
    }
}

void hal_gpio_init(gpio_conf_t *conf)
{
#ifdef DEBUG
    // Reserve GPIO
    gpio_reserve_pins(conf->gpio);
#endif

    // Port clock init
    gpio_clock_init(conf->gpio.port);

    // Set MODER
    reg_set_field(&conf->gpio.port->MODER, conf->gpio.pin << 1, 2, conf->mode);

    // Set OTYPER
    reg_set_field(&conf->gpio.port->OTYPER, conf->gpio.pin, 1, conf->type);

    // Set OSPEEDR
    reg_set_field(&conf->gpio.port->OSPEEDR, conf->gpio.pin << 1, 2, conf->speed);

    // Set PUPDR
    reg_set_field(&conf->gpio.port->PUPDR, conf->gpio.pin << 1, 2, conf->pupd);

    // Set AFRL/H
    if (conf->mode == GPIO_MODE_AF)
    {
        reg_set_field(&conf->gpio.port->AFR[conf->gpio.pin >> 3], (conf->gpio.pin & 0x7) << 2, 4,
                      conf->af);
    }
}

uint8_t hal_gpio_read_level(gpio_t gpio)
{
    return reg_get_bit(&gpio.port->IDR, gpio.pin);
}

void hal_gpio_set_level(gpio_t gpio, uint8_t level)
{
    uint8_t pos = (level == 0 ? 16 : 0) + gpio.pin;
    reg_set_bit(&gpio.port->BSRR, pos);
}
