#ifndef HAL_I2C_H
#define HAL_I2C_H

#include "hal/gpio/gpio_types.h"
#include "utils/status.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

typedef enum
{
    I2C_TYPE_TX,
    I2C_TYPE_RX,
} i2c_t;

typedef enum
{
    I2C_PERIPH_1,
    I2C_PERIPH_3,
    MAX_I2C_PERIPHERALS
} i2c_perip_t;

typedef enum
{
    I2C_SPEED_STANDARD,
    I2C_SPEED_FAST,
    I2C_SPEED_FAST_PLUS
} i2c_speed_t;

typedef struct
{
    gpio_t sda;
    gpio_t scl;
    gpio_af_t af;
    I2C_TypeDef *i2c;
    i2c_speed_t speed;
    uint8_t dnf;  // 0->15
    uint8_t irq_priority;
} i2c_conf_t;

typedef struct
{
    i2c_t type;
    I2C_TypeDef *i2c;
    uint8_t addr;

    uint8_t *buf;
    uint16_t len;
    uint16_t total;

    bool repeat;

    void (*on_complete)(bw_status_t);
} i2c_handle_t;

void hal_i2c_init(i2c_conf_t *conf);
void hal_i2c_reset(I2C_TypeDef *i2c);
void hal_i2c_ev_isr(i2c_perip_t type);
void hal_i2c_er_isr(i2c_perip_t type);
void hal_i2c_receive(i2c_handle_t *i2c_h);
void hal_i2c_transmit(i2c_handle_t *i2c_h);
void hal_i2c_transmit_receive(i2c_handle_t *i2c_h);
void hal_i2c_deinit(I2C_TypeDef *i2c);

#endif
