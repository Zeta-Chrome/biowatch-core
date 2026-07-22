#include "gpio.h"
#include "lib/assert.h"
#include "lib/utils.h"

uint8_t gpio_port_index(GPIO_TypeDef *port)
{
	return ((uint8_t *)port - (uint8_t *)GPIOA) / ((uint8_t *)GPIOB - (uint8_t *)GPIOA);
}

#ifdef DEBUG
// one bit per pin, one element per port
static uint16_t g_reserved_pins[5] = { 0 }; // A, B, C, D, E

static void gpio_reserve_pins(struct gpio gpio)
{
	uint8_t idx = gpio_port_index(gpio.port);
	uint16_t mask = (1U << gpio.pin);

	BW_ASSERT(!(g_reserved_pins[idx] & mask), "GPIO CONFLICT: Pin %d.%d already reserved", idx,
			  gpio.pin);

	g_reserved_pins[idx] |= mask;
}
#endif

static void gpio_clock_init(GPIO_TypeDef *port)
{
	if (port == GPIOA)
		SET_FIELD(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN_Msk);
	else if (port == GPIOB)
		SET_FIELD(RCC->AHB2ENR, RCC_AHB2ENR_GPIOBEN_Msk);
	else if (port == GPIOC)
		SET_FIELD(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN_Msk);
	else if (port == GPIOD)
		SET_FIELD(RCC->AHB2ENR, RCC_AHB2ENR_GPIODEN_Msk);
	else if (port == GPIOE)
		SET_FIELD(RCC->AHB2ENR, RCC_AHB2ENR_GPIOEEN_Msk);
}

void gpio_init(struct gpio_conf *conf)
{
#ifdef DEBUG
	gpio_reserve_pins(conf->gpio);
#endif

	gpio_clock_init(conf->gpio.port);
	MODIFY_FIELD_W(conf->gpio.port->MODER, 2, conf->gpio.pin << 1, conf->mode);
	MODIFY_BIT(conf->gpio.port->OTYPER, conf->gpio.pin, conf->type);
	MODIFY_FIELD_W(conf->gpio.port->OSPEEDR, 2, conf->gpio.pin << 1, conf->speed);
	MODIFY_FIELD_W(conf->gpio.port->PUPDR, 2, conf->gpio.pin << 1, conf->pupd);
	if (conf->mode == GPIO_MODE_AF)
		MODIFY_FIELD_W(conf->gpio.port->AFR[conf->gpio.pin >> 3], 4, (conf->gpio.pin & 0x7) << 2,
					   conf->af);
}

uint8_t gpio_read_level(struct gpio gpio)
{
	return READ_BIT(gpio.port->IDR, gpio.pin);
}

void gpio_set_level(struct gpio gpio, uint8_t level)
{
	uint8_t pos = (level == 0 ? 16 : 0) + gpio.pin;
	SET_BIT(gpio.port->BSRR, pos);
}

void gpio_deinit(struct gpio gpio)
{
	MODIFY_FIELD_W(gpio.port->MODER, 2, gpio.pin << 1, GPIO_MODE_ANALOG);
	CLEAR_FIELD_W(gpio.port->PUPDR, 2, gpio.pin << 1);
	CLEAR_FIELD_W(gpio.port->OSPEEDR, 2, gpio.pin << 1);
	CLEAR_BIT(gpio.port->OTYPER, gpio.pin);
	CLEAR_FIELD_W(gpio.port->AFR[gpio.pin >> 3], 4, (gpio.pin & 0x7) << 2);
}
