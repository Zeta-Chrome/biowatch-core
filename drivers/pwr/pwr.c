#include "cmsis_gcc.h"
#include "lib/utils.h"
#include "pwr.h"
#include "stm32wb55xx.h"
#include <stddef.h>

void pwr_enable_wkup(enum pwr_wkup wkup, enum pwr_wkup_edge edge)
{
	SET_FIELD(PWR->CR3, PWR_CR3_EWUP1_Msk << wkup);
	MODIFY_BIT(PWR->CR4, PWR_CR4_WP1_Pos + wkup, edge);
}

void pwr_unlock_backup_domain()
{
	SET_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
}

void pwr_lock_backup_domain()
{
	CLEAR_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
}

void pwr_enter_sleep()
{
	CLEAR_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__DSB();
	__WFI();
	__ISB();
}

void pwr_enter_lp_sleep()
{
	SET_FIELD(PWR->CR1, PWR_CR1_LPR_Msk);
	pwr_enter_sleep();
}

void pwr_exit_lp_sleep()
{
	CLEAR_FIELD(PWR->CR1, PWR_CR1_LPR_Msk);

	uint32_t timeout = 1000000;

	while ((PWR->SR2 & PWR_SR2_REGLPF_Msk) && timeout--)
		;

	if (timeout == 0) {
		__asm volatile("bkpt #0");
		BW_LOG("REGLPF timeout: SR2=0x%08lx\n", PWR->SR2);
	}
}

void pwr_enter_stop(enum pwr_stop_mode stop_mode)
{
	CLEAR_FIELD(RCC->CFGR, RCC_CFGR_STOPWUCK_Msk); // MSI oscillator
	MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, stop_mode);
	SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__DSB();
	__WFI();
	__ISB();
}

void pwr_enter_standby()
{
	MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, 0x3);
	WRITE_FIELD(PWR->SCR, PWR_SR1_WUF_Msk);
	SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__DSB();
	__WFI();
	__ISB();
}

void pwr_enter_shutdown()
{
	MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, 0x4);
	WRITE_FIELD(PWR->SCR, PWR_SR1_WUF_Msk);
	SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__DSB();
	__WFI();
	__ISB();
}
