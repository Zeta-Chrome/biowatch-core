#ifndef UTILS_DELAY_H
#define UTILS_DELAY_H

#include "drivers/clock/clock.h"
#include <stdint.h>

static inline void delay_us(uint32_t us)
{
	// (cycles per us) / 3 cycles per loop iteration
	uint32_t count = (us * (HCLK1_FREQ / 1000000UL)) / 3U;

	if (count == 0) {
		return;
	}

	__asm volatile("1: subs %[count], %[count], #1 \n\t"
				   "   bne  1b                     \n\t"
				   : [count] "+r"(count)
				   :
				   : "cc");
}

static inline void delay_ms(uint32_t ms)
{
	while (ms--)
		delay_us(1000);
}

#endif
