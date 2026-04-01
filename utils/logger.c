#ifdef DEBUG

#include "hal/uart/uart.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

static USART_TypeDef *g_uart;

void bw_logger_init(USART_TypeDef *uart, gpio_t tx)
{
    // PA9 or PB6
    g_uart = uart;
    uart_conf_t uart_conf = {.tx = tx, .af = GPIO_AF7, .uart = uart, .baud_rate = UART_BAUD_9600};
    hal_uart_init(&uart_conf);
}

void bw_log(const char *file, int line, const char *fmt, ...)
{
    char buf[200];
    char msg[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    // Strip the path
    const char *filename = strrchr(file, '/');
    filename = (filename) ? filename + 1 : file;
    int len = snprintf(buf, sizeof(buf), "[%s:%d] %s", filename, line, msg);

    if (len > 0)
    {
        char out[256];
        uint16_t out_len = 0;
        // Add \r after every \n
        for (int i = 0; i < len; i++)
        {
            if (buf[i] == '\n')
            {
                out[out_len++] = '\r';
            }
            out[out_len++] = buf[i];
        }

        hal_uart_write(g_uart, (uint8_t *)out, out_len);
    }
}

#endif
