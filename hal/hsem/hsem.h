#ifndef HAL_HSEM_H
#define HAL_HSEM_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*hsem_callback_t)(void *);

typedef struct 
{
    hsem_callback_t callback;
    void* user_data;
} hsem_handle_t; 

void hal_hsem_init(uint8_t irq_priority);
bool hal_hsem_lock(uint8_t semid, uint8_t procid, hsem_handle_t *handle);
bool hal_hsem_fast_lock(uint8_t semid, hsem_handle_t *handle);
void hal_hsem_unlock(uint8_t semid, uint8_t procid);
void hal_hsem_clear();
void hal_hsem_deinit();

#endif
