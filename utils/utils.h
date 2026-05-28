#ifndef UTILS_H
#define UTILS_H

#include "stm32wb55xx.h"
#include "hal/reg.h"
#include "status.h"
#include "logger.h"
#include "assert.h"

#define NULL ((void *)0)
#define BIT(n) (1UL << (n))
#define DIVC(x, y) (((x) + (y) - 1) / (y))

#define PACKED__ __attribute__((packed))
#define PACKED_STRUCT struct PACKED__
#define ALIGN(n) __attribute__((aligned(n)))
#define PLACE_IN_SECTION(__x__) __attribute__((section(__x__)))

static inline void bw_error_handler()
{
    __disable_irq();
    __asm volatile("bkpt #0");
    while (1);
}

static inline uint32_t dbg_mcu_get_device_id(void)
{
  return (uint32_t)(reg_get_field(&DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID_Pos, 16));
}

static inline uint32_t dbg_mcu_get_revision_id()
{
    return (uint32_t)(reg_get_field(&DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID_Pos, 16));
}

#endif
