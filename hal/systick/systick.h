#ifndef PLATFORM_TIMER_H
#define PLATFORM_TIMER_H

#include <stdint.h>

void hal_systick_init(uint8_t priority);

uint32_t hal_systick_millis();
uint32_t hal_systick_micros();

void hal_systick_delay_ms(uint32_t ms);
void hal_systick_delay_us(uint32_t ms);

#endif
