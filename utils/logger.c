#ifdef DEBUG

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#if defined(UART_LOGGER)
#include "hal/uart/uart.h"
#define PL_UART USART1
static const gpio_t PL_UART_TX = {.port = GPIOA, .pin = 9};  // PA9
#elif defined(RTT_LOGGER)
#include "rtt/SEGGER_RTT.h"
#endif

void bw_logger_init()
{
#if defined(UART_LOGGER)
    // PA9 or PB6
    uart_conf_t uart_conf = {
    .tx = PL_UART_TX, .af = GPIO_AF7, .uart = PL_UART, .baud_rate = UART_BAUD_9600};
    hal_uart_init(&uart_conf);
#elif defined(RTT_LOGGER)
    SEGGER_RTT_Init();
#endif
}

void bw_log(const char *file, int line, const char *fmt, ...)
{
    static char msg[128];
    static char buf[256];

    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    const char *filename = strrchr(file, '/');
    filename = (filename) ? filename + 1 : file;
    int buf_len = snprintf(buf, sizeof(buf), "[%s:%d] %s", filename, line, msg);

    if (buf_len <= 0)
        return; 

#if defined(UART_LOGGER)
    static char out[256];
    uint16_t out_len = 0;

    for (int i = 0; i < buf_len; i++)
    {
        if (buf[i] == '\n')
        {
            out[out_len++] = '\r';
        }
        out[out_len++] = buf[i];
    }
    out[out_len] = '\0';

    hal_uart_write(PL_UART, (uint8_t *)out, out_len);
#elif defined(RTT_LOGGER)
    SEGGER_RTT_Write(0, buf, buf_len);
#endif
}

#endif
