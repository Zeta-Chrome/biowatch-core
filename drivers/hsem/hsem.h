#ifndef DRIVER_HSEM_H
#define DRIVER_HSEM_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*hsem_callback_t)(void *);

struct hsem_handle {
	hsem_callback_t callback;
	void *user_data;
};

void hsem_init(uint8_t irq_priority);
bool hsem_lock(uint8_t semid, uint8_t procid, struct hsem_handle *handle);
bool hsem_fast_lock(uint8_t semid, struct hsem_handle *handle);
void hsem_unlock(uint8_t semid, uint8_t procid);
void hsem_clear();
void hsem_deinit();

#endif
