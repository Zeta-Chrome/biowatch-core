#include "hsem.h"
#include "cmsis_gcc.h"
#include "lib/assert.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"

#define HSEM_CLR_KEY 0xA5

static struct hsem_handle *g_handles[32];

void hsem_init(uint8_t irq_priority)
{
	HSEM->KEYR = HSEM_CLR_KEY << 16;
	NVIC_SetPriority(HSEM_IRQn, irq_priority);
	NVIC_EnableIRQ(HSEM_IRQn);
}

void hsem_lock_spin(uint8_t semid, uint8_t procid)
{
	BW_ASSERT(semid < 32, "Invalid semaphore %d (Expected 0-31)", semid);

	uint32_t sem = HSEM_R_LOCK_Msk | HSEM_CPU1_COREID << HSEM_R_COREID_Pos | procid;
	do {
		HSEM->R[semid] = sem;
	} while (HSEM->R[semid] != sem);
}

bool hsem_lock(uint8_t semid, uint8_t procid, struct hsem_handle *handle)
{
	BW_ASSERT(semid < 32, "Invalid semaphore %d (Expected 0-31)", semid);

	uint32_t sem = HSEM_R_LOCK_Msk | HSEM_CPU1_COREID << HSEM_R_COREID_Pos | procid;
	HSEM->R[semid] = sem;

	if (HSEM->R[semid] != sem) {
		g_handles[semid] = handle;
		SET_BIT(HSEM->C1IER, HSEM_C1IER_ISE0_Pos + semid);
		if (HSEM->R[semid] == sem) { // To prevent race condition
			CLEAR_BIT(HSEM->C1IER, HSEM_C1IER_ISE0_Pos + semid);
			return true;
		}
		return false;
	}

	return true;
}

void hsem_fast_lock_spin(uint8_t semid)
{
	BW_ASSERT(semid < 32, "Invalid semaphore %d (Expected 0-31)", semid);

	uint32_t sem = (HSEM_R_LOCK_Msk | HSEM_CPU1_COREID << HSEM_R_COREID_Pos);
	while (HSEM->RLR[semid] != sem)
		__NOP();
}

bool hsem_fast_lock(uint8_t semid, struct hsem_handle *handle)
{
	BW_ASSERT(semid < 32, "Invalid semaphore %d (Expected 0-31)", semid);

	uint32_t sem = (HSEM_R_LOCK_Msk | HSEM_CPU1_COREID << HSEM_R_COREID_Pos);
	if (HSEM->RLR[semid] != sem) {
		g_handles[semid] = handle;
		SET_BIT(HSEM->C1IER, HSEM_C1IER_ISE0_Pos + semid);
		if (HSEM->RLR[semid] == sem) { // To prevent race condition
			CLEAR_BIT(HSEM->C1IER, HSEM_C1IER_ISE0_Pos + semid);
			return true;
		}
		return false;
	}

	return true;
}

void hsem_isr()
{
	uint8_t idx;
	uint32_t mask = HSEM->C1ISR & HSEM->C1IER;
	while (mask != 0) {
		idx = __CLZ(__RBIT(mask));
		mask = mask & (mask - 1);
		WRITE_BIT(HSEM->C1ICR, HSEM_C1ICR_ISC0_Pos + idx);
		if (g_handles[idx]->callback)
			g_handles[idx]->callback(g_handles[idx]->user_data);
	}
}

void hsem_unlock(uint8_t semid, uint8_t procid)
{
	BW_ASSERT(semid < 32, "Invalid semaphore %d (Expected 0-31)", semid);

	uint32_t sem = HSEM_CPU1_COREID << HSEM_R_COREID_Pos | procid;
	HSEM->R[semid] = sem;
}

void hsem_fast_unlock(uint8_t semid)
{
	BW_ASSERT(semid < 32, "Invalid semaphore %d (Expected 0-31)", semid);

	uint32_t sem = HSEM_CPU1_COREID << HSEM_R_COREID_Pos;
	HSEM->R[semid] = sem;
}

void hsem_clear()
{
	HSEM->CR = (HSEM_CLR_KEY << 16) | (HSEM_CPU1_COREID << 8);
}

void hsem_deinit()
{
	NVIC_DisableIRQ(HSEM_IRQn);
}
