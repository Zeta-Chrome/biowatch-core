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

#define PUTC(msg, msg_len, ch)                                                                          \
    do                                                                                                  \
    {                                                                                                   \
        if ((msg_len) > 1)                                                                              \
        {                                                                                               \
            *(msg)++ = ch;                                                                              \
            (msg_len)--;                                                                                \
        }                                                                                               \
    } while (0)

static char g_msg[128];

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

static inline void put_digits(char **msg, int *msg_len, uint32_t d, int base)
{
    if (d == 0)
    {
        PUTC(*msg, *msg_len, '0');
        return;
    }

    int i = 0;
    char buf[12];
    const char *digits = "0123456789ABCDEF";

    while (d > 0)
    {
        buf[i++] = digits[d % base];
        d /= base;
    }
    while (i--)
    {
        PUTC(*msg, *msg_len, buf[i]);
    }
}

static int str_formatv(char *msg, int msg_len, const char *fmt, va_list args)
{
    int len = msg_len;
    while (*fmt && msg_len > 1)
    {
        if (*fmt != '%')
        {
            PUTC(msg, msg_len, *fmt++);
            continue;
        }

        fmt++;

        switch (*fmt++)
        {
        case 's':
        {
            const char *s = va_arg(args, const char *);
            while (*s)
            {
                PUTC(msg, msg_len, *s);
                s++;
            }
            break;
        }
        case 'd':
        {
            int d = va_arg(args, int);
            if (d < 0)
            {
                PUTC(msg, msg_len, '-');
            }
            put_digits(&msg, &msg_len, (d < 0) ? -d : d, 10);
            break;
        }
        case 'u':
        {
            int u = va_arg(args, uint32_t);
            put_digits(&msg, &msg_len, u, 10);
            break;
        }
        case 'x':
        {
            int u = va_arg(args, uint32_t);
            PUTC(msg, msg_len, '0');
            PUTC(msg, msg_len, 'x');
            put_digits(&msg, &msg_len, u, 16);
            break;
        }
        case 'p':
        {
            uint32_t p = (uint32_t)va_arg(args, void *);
            PUTC(msg, msg_len, '0');
            PUTC(msg, msg_len, 'x');
            put_digits(&msg, &msg_len, p, 16);
            break;
        }
        case 'c':
        {
            char c = va_arg(args, int);
            PUTC(msg, msg_len, c);
            break;
        }
        default:
        {
            PUTC(msg, msg_len, '%');
            break;
        }
        }
    }

    *msg = '\0';

    return len - msg_len;
}

static int str_format(char *msg, int msg_len, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int str_len = str_formatv(msg, msg_len, fmt, args);
    va_end(args);

    return str_len;
}

static void print(const char *msg, int msg_len)
{
    if (msg_len <= 0)
        return;

#if defined(UART_LOGGER)
    char out[256];
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
    SEGGER_RTT_Write(0, msg, msg_len);
#endif
}

void bw_log(const char *file, int line, const char *fmt, ...)
{
    const char *filename = strrchr(file, '/');
    filename = (filename) ? filename + 1 : file;
    int msg_len = str_format(g_msg, sizeof(g_msg), "[%s:%d]", filename, line);

    va_list args;
    va_start(args, fmt);
    msg_len += str_formatv(g_msg + msg_len, sizeof(g_msg) - msg_len, fmt, args);
    va_end(args);

    print(g_msg, msg_len);
}

void bw_print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int msg_len = str_formatv(g_msg, sizeof(g_msg), fmt, args);
    va_end(args);

    print(g_msg, msg_len);
}

#endif
