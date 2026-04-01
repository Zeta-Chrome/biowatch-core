#ifndef HAL_GPIO_TYPES_H
#define HAL_GPIO_TYPES_H

#include "stm32wb55xx.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint8_t pin;
} gpio_t;

typedef enum
{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} gpio_mode_t;

typedef enum
{
    GPIO_TYPE_PUSH_PULL,
    GPIO_TYPE_OPEN_DRAIN
} gpio_type_t;

typedef enum
{
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_FAST,
    GPIO_SPEED_HIGH
} gpio_speed_t;

typedef enum
{
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pupd_t;

typedef enum
{
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
} gpio_af_t;

typedef struct
{
    gpio_t gpio;
    gpio_mode_t mode;
    gpio_type_t type;
    gpio_speed_t speed;
    gpio_pupd_t pupd;
    gpio_af_t af;
} gpio_conf_t;

// default functions
static inline gpio_conf_t gpio_conf_output(gpio_t gpio, gpio_speed_t speed)
{
    return (gpio_conf_t){
    .gpio = gpio,
    .mode = GPIO_MODE_OUTPUT,
    .type = GPIO_TYPE_PUSH_PULL,
    .speed = speed,
    .pupd = GPIO_PULL_NONE,
    .af = 0,
    };
}

static inline gpio_conf_t gpio_conf_input(gpio_t gpio, gpio_pupd_t pupd)
{
    return (gpio_conf_t){
    .gpio = gpio,
    .mode = GPIO_MODE_INPUT,
    .type = 0,
    .speed = GPIO_SPEED_LOW,
    .pupd = pupd,
    .af = 0,
    };
}

static inline gpio_conf_t gpio_conf_af(gpio_t gpio, gpio_af_t af, gpio_type_t type, gpio_pupd_t pupd)
{
    return (gpio_conf_t){
    .gpio = gpio,
    .mode = GPIO_MODE_AF,
    .type = type,
    .speed = GPIO_SPEED_HIGH,
    .pupd = pupd,
    .af = af,
    };
}

static inline gpio_conf_t gpio_conf_analog(gpio_t gpio)
{
    return (gpio_conf_t){
    .gpio = gpio,
    .mode = GPIO_MODE_ANALOG,
    .pupd = GPIO_PULL_NONE,
    };
}

#endif
