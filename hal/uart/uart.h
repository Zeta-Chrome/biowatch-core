#ifndef HAL_UART_H
#define HAL_UART_H

#include "hal/gpio/gpio_types.h"
#include "stm32wb55xx.h"
#include <stdint.h>

#define USART_KER_CK SystemCoreClock 
#define USART_PRESC 0

typedef enum
{
    UART_BAUD_9600 = 9600,
    UART_BAUD_19200 = 19200,
    UART_BAUD_38400 = 38400,
    UART_BAUD_57600 = 57600,
    UART_BAUD_115200 = 115200,
} uart_baud_t;

typedef struct
{
    gpio_t tx;
    gpio_af_t af;
    USART_TypeDef *uart;
    uart_baud_t baud_rate;
} uart_conf_t;


void hal_uart_init(uart_conf_t *conf);
void hal_uart_write(USART_TypeDef *uart, uint8_t* data, uint16_t len);
void hal_uart_deinit(USART_TypeDef *uart);

#endif
