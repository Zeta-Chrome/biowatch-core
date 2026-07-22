#ifndef UTILS_H
#define UTILS_H

#include "assert.h" // IWYU pragma: export
#include "logger.h" // IWYU pragma: export
#include "status.h" // IWYU pragma: export
#include "stm32wb55xx.h"

#define BIT(n) (1UL << (n))
#define MASK(w, p) (((1UL << (w)) - 1UL) << (p))

#define DIVC(x, y) (((x) + (y) - 1) / (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define PACKED__ __attribute__((packed))
#define PACKED_STRUCT struct PACKED__
#define ALIGN(n) __attribute__((aligned(n)))
#define PLACE_IN_SECTION(__x__) __attribute__((section(__x__)))

// READ OPERATIONS
#define READ_BIT(REG, POS) (((REG) >> (POS)) & 1UL)

#define READ_FIELD(REG, MASK, POS) (((REG) & (MASK)) >> (POS))

// WRITE OPERATIONS
#define SET_BIT(REG, POS) ((REG) |= (1UL << (POS)))
#define CLEAR_BIT(REG, POS) ((REG) &= ~(1UL << (POS)))
#define WRITE_BIT(REG, POS) ((REG) = (1UL << (POS)))
#define TOGGLE_BIT(REG, POS) ((REG) ^= (1UL << (POS))
#define MODIFY_BIT(REG, POS, VAL) ((REG) = ((REG) & ~(1UL << (POS))) | (((VAL) & 1UL) << (POS)))

#define SET_FIELD(REG, MASK) ((REG) |= (MASK))
#define SET_FIELD_W(REG, MASK) SET_FIELD(REG, MASK(WIDTH, POS))
#define CLEAR_FIELD(REG, MASK) ((REG) &= ~(MASK))
#define CLEAR_FIELD_W(REG, WIDTH, POS) CLEAR_FIELD(REG, MASK(WIDTH, POS))
#define WRITE_FIELD(REG, MASK) ((REG) = (MASK))
#define WRITE_FIELD_W(REG, WIDTH, POS) WRITE_FIELD(REG, MASK(WIDTH, POS))
#define MODIFY_FIELD(REG, MASK, POS, VAL) ((REG) = ((REG) & ~(MASK)) | (((VAL) << (POS)) & (MASK)))
#define MODIFY_FIELD_W(REG, WIDTH, POS, VAL) MODIFY_FIELD(REG, MASK(WIDTH, POS), POS, VAL)

static inline void bw_error_handler()
{
	__disable_irq();
	__asm volatile("bkpt #0");
	while (1)
		;
}

static inline uint32_t dbg_mcu_get_device_id(void)
{
	return (
		uint32_t)(READ_FIELD(DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID_Msk, DBGMCU_IDCODE_DEV_ID_Pos));
}

static inline uint32_t dbg_mcu_get_revision_id()
{
	return (
		uint32_t)(READ_FIELD(DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID_Msk, DBGMCU_IDCODE_REV_ID_Pos));
}

#endif
