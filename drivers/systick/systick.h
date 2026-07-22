#ifndef DRIVER_SYSTICK_H
#define DRIVER_SYSTICK_H

#include <stdint.h>

void systick_init(uint8_t priority);

uint32_t systick_millis();
uint64_t systick_micros();

void systick_delay_ms(uint32_t ms);
void systick_delay_us(uint32_t ms);

#endif
