#ifndef DRIVER_UART_H
#define DRIVER_UART_H

#include "drivers/gpio/gpio_types.h"
#include "stm32wb55xx.h"
#include <stdint.h>

enum uart_baud {
	UART_BAUD_9600 = 9600,
	UART_BAUD_19200 = 19200,
	UART_BAUD_38400 = 38400,
	UART_BAUD_57600 = 57600,
	UART_BAUD_115200 = 115200,
};

struct uart_conf {
	struct gpio tx;
	enum gpio_af af;
	USART_TypeDef *uart;
	enum uart_baud baud_rate;
};

void uart_init(struct uart_conf *conf);
void uart_write(USART_TypeDef *uart, uint8_t *data, uint16_t len);
void uart_deinit(USART_TypeDef *uart);

#endif
