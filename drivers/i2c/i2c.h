#ifndef DRIVER_I2C_H
#define DRIVER_I2C_H

#include "drivers/gpio/gpio_types.h"
#include "lib/status.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

typedef void (*i2c_callback_t)(enum bw_status, void *);

enum i2c {
	I2C_TYPE_TX,
	I2C_TYPE_RX,
};

enum i2c_perip { I2C_PERIPH_1, I2C_PERIPH_3, MAX_I2C_PERIPHERALS };

enum i2c_speed { I2C_SPEED_LOW, I2C_SPEED_STANDARD, I2C_SPEED_FAST, I2C_SPEED_FAST_PLUS };

struct i2c_conf {
	struct gpio sda;
	struct gpio scl;
	enum gpio_af af;
	I2C_TypeDef *i2c;
	enum i2c_speed speed;
	uint8_t dnf; // 0->15
	uint8_t irq_priority;
};

struct i2c_handle {
	enum i2c type;
	I2C_TypeDef *i2c;
	enum i2c_perip perip;
	bool dma_mode;
	uint8_t addr;
	uint8_t *buf;
	uint16_t len;
	uint16_t remaining;
	bool repeat;
	i2c_callback_t callback;
	void *user_data;
};

void i2c_init(struct i2c_conf *conf, struct i2c_handle *handle);
void i2c_init_dma(struct i2c_conf *conf, struct i2c_handle *handle);
void i2c_reset(struct i2c_handle *handle);
void i2c_reset_dma(struct i2c_handle *handle);
void i2c_receive(struct i2c_handle *handle);
void i2c_receive_dma(struct i2c_handle *handle);
void i2c_transmit(struct i2c_handle *handle);
void i2c_transmit_dma(struct i2c_handle *handle);
void i2c_deinit(struct i2c_handle *handle);
void i2c_deinit_dma(struct i2c_handle *handle);

#endif
