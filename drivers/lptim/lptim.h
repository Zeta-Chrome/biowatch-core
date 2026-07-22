#ifndef DRIVER_LPTIM_H
#define DRIVER_LPTIM_H

#include <stdint.h>

typedef void (*lptim_callback_t)(void *);

struct lptim_conf {
	uint8_t priority;
	lptim_callback_t callback;
	void *user_data;
};

void lptim_init(struct lptim_conf *conf);
void lptim_trigger_period(uint16_t ms);
void lptim_trigger_once(uint16_t ms);
void lptim_deinit();

#endif
