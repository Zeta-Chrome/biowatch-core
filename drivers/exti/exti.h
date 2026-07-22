#ifndef DRIVER_EXTI_H
#define DRIVER_EXTI_H

#include "drivers/gpio/gpio_types.h"
#include "stm32wb55xx.h"

enum exti_edge {
	EXTI_EDGE_RISING = 0x1,
	EXTI_EDGE_FALLING = 0x2,
	EXTI_EDGE_BOTH = 0x3,
};

typedef void (*exti_callback_t)(void *user_data);

struct exti_conf {
	union {
		// Both pin and im occupy the same space
		struct {
			struct gpio gpio; // 1 byte of pin + 8 bytes of GPIO_Typedef *
			enum gpio_pupd pupd;
		};
		uint8_t im; // 1 byte of im
	};
	enum exti_edge edge;
	IRQn_Type irq;
	uint8_t irq_priority; // 0-15
	exti_callback_t callback;
	void *user_data;
};

struct exti_handle {
	union {
		struct {
			struct gpio gpio;
			enum gpio_pupd pupd;
		};

		uint8_t im;
	};
	IRQn_Type irq;
	exti_callback_t callback;
	void *user_data;
};

void exti_init(struct exti_conf *conf, struct exti_handle *handle);
void exti_enable_line(uint8_t im);
void exti_gpio_init(struct exti_conf *conf, struct exti_handle *handle);
void exti_deinit(struct exti_handle *handle);
void exti_gpio_deinit(struct exti_handle *handle);

#endif
