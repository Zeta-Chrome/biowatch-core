#ifndef DRIVER_GPIO_TYPES_H
#define DRIVER_GPIO_TYPES_H

#include "stm32wb55xx.h"

struct gpio {
	uint8_t pin;
	GPIO_TypeDef *port;
};

enum gpio_mode { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

enum gpio_type { GPIO_TYPE_PUSH_PULL, GPIO_TYPE_OPEN_DRAIN };

enum gpio_speed { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_FAST, GPIO_SPEED_HIGH };

enum gpio_pupd { GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN };

enum gpio_af {
	GPIO_AF0,
	GPIO_AF1,
	GPIO_AF2,
	GPIO_AF3,
	GPIO_AF4,
	GPIO_AF5,
	GPIO_AF6,
	GPIO_AF7,
	GPIO_AF8,
	GPIO_AF9,
	GPIO_AF10,
	GPIO_AF11,
	GPIO_AF12,
	GPIO_AF13,
	GPIO_AF14,
	GPIO_AF15
};

struct gpio_conf {
	struct gpio gpio;
	enum gpio_mode mode;
	enum gpio_type type;
	enum gpio_speed speed;
	enum gpio_pupd pupd;
	enum gpio_af af;
};

// default functions
static inline struct gpio_conf gpio_conf_output(struct gpio gpio, enum gpio_speed speed)
{
	return (struct gpio_conf){
		.gpio = gpio,
		.mode = GPIO_MODE_OUTPUT,
		.type = GPIO_TYPE_PUSH_PULL,
		.speed = speed,
		.pupd = GPIO_PULL_NONE,
		.af = 0,
	};
}

static inline struct gpio_conf gpio_conf_input(struct gpio gpio, enum gpio_pupd pupd)
{
	return (struct gpio_conf){
		.gpio = gpio,
		.mode = GPIO_MODE_INPUT,
		.type = 0,
		.speed = GPIO_SPEED_LOW,
		.pupd = pupd,
		.af = 0,
	};
}

static inline struct gpio_conf gpio_conf_af(struct gpio gpio, enum gpio_af af, enum gpio_type type,
											enum gpio_pupd pupd)
{
	return (struct gpio_conf){
		.gpio = gpio,
		.mode = GPIO_MODE_AF,
		.type = type,
		.speed = GPIO_SPEED_HIGH,
		.pupd = pupd,
		.af = af,
	};
}

static inline struct gpio_conf gpio_conf_analog(struct gpio gpio)
{
	return (struct gpio_conf){
		.gpio = gpio,
		.mode = GPIO_MODE_ANALOG,
		.pupd = GPIO_PULL_NONE,
	};
}

#endif
