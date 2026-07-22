#include "drivers/gpio/gpio.h"
#include "exti.h"
#include "lib/assert.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"

#define NULL ((void *)0)
#define MAX_EXTI_LINES 50

static struct exti_handle *g_exti_handles[MAX_EXTI_LINES];

uint8_t gpio_port_index(GPIO_TypeDef *port);

void exti_init(struct exti_conf *conf, struct exti_handle *handle)
{
	BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)",
			  conf->irq_priority);

	// Enable rising or falling edge
	if ((conf->edge & EXTI_EDGE_RISING) != 0)
		SET_BIT(EXTI->RTSR1, conf->im);

	if ((conf->edge & EXTI_EDGE_FALLING) != 0)
		SET_BIT(EXTI->FTSR1, conf->im);

	EXTI->PR1 = (1U << conf->im);

	// Unmask interrupt line
	exti_enable_line(conf->im);

	handle->im = conf->im;
	handle->irq = conf->irq;
	handle->callback = conf->callback;
	handle->user_data = conf->user_data;
	g_exti_handles[conf->im] = handle;

	// Enable interrupt
	NVIC_EnableIRQ(conf->irq);
	NVIC_SetPriority(conf->irq, conf->irq_priority);
}

void exti_enable_line(uint8_t im)
{
	BW_ASSERT(im < 48, "Invalid exti line %d (Expected range 0-47)", im);

	if (im < 32)
		SET_BIT(EXTI->IMR1, im);
	else
		SET_BIT(EXTI->IMR2, im - 32);
}

void exti_gpio_init(struct exti_conf *conf, struct exti_handle *handle)
{
	struct gpio_conf int_conf = gpio_conf_input(conf->gpio, conf->pupd);
	gpio_init(&int_conf);

	uint8_t port = gpio_port_index(conf->gpio.port);
	uint8_t pin = conf->gpio.pin;
	MODIFY_FIELD_W(SYSCFG->EXTICR[pin / 4], 3, (pin % 4) * 4, port);
	exti_init(conf, handle);
	handle->gpio = conf->gpio;
}

void exti_isr(uint8_t im)
{
	EXTI->PR1 = (1U << im);

	if (g_exti_handles[im]->callback != NULL)
		g_exti_handles[im]->callback(g_exti_handles[im]->user_data);
}

void exti_deinit(struct exti_handle *handle)
{
	CLEAR_BIT(EXTI->IMR1, handle->im);
	CLEAR_BIT(EXTI->FTSR1, handle->im);
	CLEAR_BIT(EXTI->RTSR1, handle->im);
	SET_BIT(EXTI->PR1, handle->im);
	g_exti_handles[handle->im]->callback = NULL;
	NVIC_DisableIRQ(g_exti_handles[handle->im]->irq);
}

void exti_gpio_deinit(struct exti_handle *handle)
{
	uint8_t pin = handle->gpio.pin;
	MODIFY_FIELD_W(SYSCFG->EXTICR[pin / 4], 3, (pin % 4) * 4, 0);
	exti_deinit(handle);
	gpio_deinit(handle->gpio);
}
