#include "stdbool.h"
#ifdef DEBUG

#include "kernel/critical.h"
#include "logger.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(UART_LOGGER)
#include "drivers/uart/uart.h"
#define PL_UART USART1
static const struct gpio PL_UART_TX = { .port = GPIOA, .pin = 9 }; // PA9
#elif defined(RTT_LOGGER)
#include "lib/rtt/SEGGER_RTT.h"
#endif

#define PUTC(msg, msg_len, ch) \
	do {                       \
		if ((msg_len) > 1) {   \
			*(msg)++ = ch;     \
			(msg_len)--;       \
		}                      \
	} while (0)

union ieee754_double {
	double f;
	uint64_t raw;
	struct {
		uint64_t mantissa : 52;
		uint64_t exponent : 11;
		uint64_t sign : 1;
	} bits;
};

static char g_msg[128];

void bw_logger_init()
{
#if defined(UART_LOGGER)
	// PA9 or PB6
	struct uart_conf uart_conf = {
		.tx = PL_UART_TX, .af = GPIO_AF7, .uart = PL_UART, .baud_rate = UART_BAUD_9600
	};
	uart_init(&uart_conf);
#elif defined(RTT_LOGGER)
	SEGGER_RTT_Init();
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#endif
}

static inline void put_str(char **msg, int *msg_len, const char *s)
{
	while (*s) {
		PUTC(*msg, *msg_len, *s);
		s++;
	}
}

static inline void put_digits(char **msg, int *msg_len, uint64_t d, int base)
{
	if (d == 0) {
		PUTC(*msg, *msg_len, '0');
		return;
	}

	int i = 0;
	char buf[20];
	const char *digits = "0123456789ABCDEF";

	while (d > 0) {
		buf[i++] = digits[d % base];
		d /= base;
	}

	while (i--)
		PUTC(*msg, *msg_len, buf[i]);
}

static inline void put_float(char **msg, int *msg_len, union ieee754_double *df)
{
	uint16_t raw_exp = df->bits.exponent; /* 0..2047, no bias subtracted */
	uint64_t mantissa = df->bits.mantissa;
	uint8_t sign = df->bits.sign;

	if (raw_exp == 0x7FF) /* inf / nan */
	{
		if (mantissa == 0)
			put_str(msg, msg_len, sign ? "-inf" : "inf");
		else
			put_str(msg, msg_len, "nan");

		return;
	}

	if (sign)
		PUTC(*msg, *msg_len, '-');

	double f = df->f;
	if (f < 0)
		f = -f;

	uint32_t int_part = (uint32_t)f;
	double frac = f - (double)int_part;

	put_digits(msg, msg_len, int_part, 10);

	/* how many digits did the integer part use? */
	uint32_t tmp = int_part;
	int int_digits = (tmp == 0) ? 1 : 0;
	while (tmp > 0) {
		int_digits++;
		tmp /= 10;
	}

	int max_frac = 5;
	int budget = 10 - int_digits;
	if (budget < max_frac)
		max_frac = budget;
	if (max_frac < 0)
		max_frac = 0;

	char digits[5];
	int n = 0;
	for (int i = 0; i < max_frac; i++) {
		frac *= 10;
		uint32_t d = (uint32_t)frac;
		digits[n++] = (char)('0' + d);
		frac -= d;
	}

	while (n > 0 && digits[n - 1] == '0') // trim trailing zeros
		n--;

	if (n > 0) {
		PUTC(*msg, *msg_len, '.');
		for (int i = 0; i < n; i++)
			PUTC(*msg, *msg_len, digits[i]);
	}
}

static int str_formatv(char *msg, int msg_len, const char *fmt, va_list args)
{
	int len = msg_len;
	while (*fmt && msg_len > 1) {
		if (*fmt != '%') {
			PUTC(msg, msg_len, *fmt++);
			continue;
		}

		fmt++;

		bool is_long = false;
		if (*fmt == 'l') {
			fmt++;
			is_long = true;
			if (*fmt == 'l')
				fmt++;
		}

		switch (*fmt++) {
		case 's': {
			const char *s = va_arg(args, const char *);
			put_str(&msg, &msg_len, s);
			break;
		}
		case 'd': {
			int64_t d;
			if (is_long)
				d = va_arg(args, int64_t);
			else
				d = va_arg(args, int32_t);

			if (d < 0)
				PUTC(msg, msg_len, '-');

			put_digits(&msg, &msg_len, (d < 0) ? -d : d, 10);
			break;
		}
		case 'u': {
			uint64_t u;
			if (is_long)
				u = va_arg(args, uint64_t);
			else
				u = va_arg(args, uint32_t);

			put_digits(&msg, &msg_len, u, 10);
			break;
		}
		case 'f': {
			double df = va_arg(args, double);
			put_float(&msg, &msg_len, (union ieee754_double *)&df);
			break;
		}
		case 'x': {
			uint32_t u = va_arg(args, uint32_t);
			PUTC(msg, msg_len, '0');
			PUTC(msg, msg_len, 'x');
			put_digits(&msg, &msg_len, u, 16);
			break;
		}
		case 'p': {
			uint32_t p = (uint32_t)va_arg(args, void *);
			PUTC(msg, msg_len, '0');
			PUTC(msg, msg_len, 'x');
			put_digits(&msg, &msg_len, p, 16);
			break;
		}
		case 'c': {
			char c = va_arg(args, int);
			PUTC(msg, msg_len, c);
			break;
		}
		default: {
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

void bw_print_s(const char *msg, int msg_len)
{
	if (msg_len <= 0)
		return;

#if defined(UART_LOGGER)
	char out[256];
	uint16_t out_len = 0;

	for (int i = 0; i < out_len; i++) {
		if (out[i] == '\n')
			out[out_len++] = '\r';

		out[out_len++] = out[i];
	}
	out[out_len] = '\0';

	uart_write(PL_UART, (uint8_t *)out, out_len);
#elif defined(RTT_LOGGER)
	SEGGER_RTT_Write(0, msg, msg_len);
#endif
}

void bw_log(const char *file, int line, const char *fmt, ...)
{
	KERNEL_ENTER_CRITICAL();

	const char *filename = strrchr(file, '/');
	filename = (filename) ? filename + 1 : file;
	int msg_len = str_format(g_msg, sizeof(g_msg), "[%s:%d]", filename, line);

	va_list args;
	va_start(args, fmt);
	msg_len += str_formatv(g_msg + msg_len, (int)sizeof(g_msg) - msg_len, fmt, args);
	va_end(args);

	bw_print_s(g_msg, msg_len);

	KERNEL_EXIT_CRITICAL();
}

void bw_print(const char *fmt, ...)
{
	KERNEL_ENTER_CRITICAL();

	va_list args;
	va_start(args, fmt);
	int msg_len = str_formatv(g_msg, sizeof(g_msg), fmt, args);
	va_end(args);

	bw_print_s(g_msg, msg_len);

	KERNEL_EXIT_CRITICAL();
}

#endif
