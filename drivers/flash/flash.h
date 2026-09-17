#ifndef DRIVER_FLASH_H
#define DRIVER_FLASH_H

#include <stdint.h>

void flash_unlock();
void flash_lock();
void flash_erase_page(uint8_t page_no);
void flash_program(uint32_t dst_addr, const uint64_t *data, uint32_t size);
void flash_configure_latency(uint32_t freq);

#endif
