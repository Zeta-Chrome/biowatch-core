#ifndef KERNEL_CRITICAL_H
#define KERNEL_CRITICAL_H

#include <stdint.h>
#include "cmsis_gcc.h"
#include "stm32wb55xx.h"

#define HIGHEST_IRQ_PRIO 3
#define KERNEL_ENTER_CRITICAL() kernel_enter_critical()
#define KERNEL_EXIT_CRITICAL(key) kernel_exit_critical(key)

static inline uint32_t kernel_enter_critical()
{
	uint32_t basepri = __get_BASEPRI();
	__set_BASEPRI_MAX(HIGHEST_IRQ_PRIO << (8 - __NVIC_PRIO_BITS));
	__DSB();
	__ISB();
	return basepri;
}

static inline void kernel_exit_critical(uint32_t basepri)
{
	__DSB();
	__ISB();
	__set_BASEPRI(basepri);
}

#endif
#ifndef KERNEL_CRITICAL_H
#define KERNEL_CRITICAL_H

#include <stdint.h>
#include "cmsis_gcc.h"
#include "stm32wb55xx.h"

#define KERNEL_ENTER_CRITICAL() kernel_enter_critical()
#define KERNEL_EXIT_CRITICAL(key) kernel_exit_critical(key)

static inline uint32_t kernel_enter_critical(void)
{
	// Save current PRIMASK state (0 = interrupts enabled, 1 = masked)
	uint32_t primask = __get_PRIMASK();

	// Mask all configurable interrupts (CPSID I)
	__disable_irq();
	__DSB();
	__ISB();

	return primask;
}

static inline void kernel_exit_critical(uint32_t primask)
{
	__DSB();
	__ISB();
	// Restore previous PRIMASK state (CPSIE I if it was 0)
	__set_PRIMASK(primask);
}

#endif /* KERNEL_CRITICAL_H */
