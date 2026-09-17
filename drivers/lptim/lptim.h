#ifndef DRIVER_LPTIM_H
#define DRIVER_LPTIM_H

#include <stdint.h>

typedef void (*lptim_callback_t)(uint32_t ticks);

struct lptim_conf {};

void lptim_init(uint8_t priority);
void lptim_trigger_period(uint16_t ms, lptim_callback_t callback);
void lptim_trigger_once(uint16_t ms, lptim_callback_t callback);
uint32_t lptim_get_elapsed_ms();
void lptim_stop_trigger();
void lptim_deinit();

#endif
