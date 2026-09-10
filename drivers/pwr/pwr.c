#include "drivers/clock/clock.h"
#include "lib/utils.h"
#include "pwr.h"
#include "stm32wb55xx.h"
#include <stddef.h>

void pwr_enable_wkup(enum pwr_wkup wkup, enum pwr_wkup_edge edge)
{
	SET_FIELD(PWR->CR3, PWR_CR3_EWUP1_Msk << wkup);
	MODIFY_FIELD_W(PWR->CR4, 1, PWR_CR4_WP1_Pos + wkup, edge);
}

void pwr_unlock_backup_domain()
{
	SET_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);

	// Unlock the write protections by writing keys
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
}

void pwr_lock_backup_domain()
{
	MODIFY_FIELD(RTC->WPR, RTC_WPR_KEY_Msk, RTC_WPR_KEY_Pos, 0x00);
	CLEAR_FIELD(PWR->CR1, PWR_CR1_DBP_Msk);
}

void pwr_enter_sleep()
{
	CLEAR_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__WFI();
}

void pwr_enter_lp_sleep()
{
	// Configure clock frequence below 2MHz
	struct clock_conf conf = clock_conf_lp_sleep();
	clock_reconfigure(&conf);
	SET_FIELD(PWR->CR1, PWR_CR1_LPR_Msk);
	pwr_enter_sleep();
}

void pwr_exit_lp_sleep()
{
	// Clear LPR
	CLEAR_FIELD(PWR->CR1, PWR_CR1_LPR_Msk);
	while (PWR->SR2 & PWR_SR2_REGLPF_Msk)
		;

	// Increase clock
	struct clock_conf conf = clock_conf_performance();
	clock_reconfigure(&conf);
}

void pwr_enter_stop(enum pwr_stop_mode stop_mode)
{
	// Configure clock to MSI 48Mhz
	struct clock_conf conf = clock_conf_stop();
	clock_reconfigure(&conf);

	MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, stop_mode);
	SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__WFI();
}

void pwr_exit_stop()
{
	struct clock_conf conf = clock_conf_performance();
	clock_reconfigure(&conf);
}

void pwr_enter_standby()
{
	MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, 0x3);
	CLEAR_FIELD(PWR->SR1, PWR_SR1_WUF_Msk);
	SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__WFI();
}

void pwr_enter_shutdown()
{
	MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, 0x4);
	CLEAR_FIELD(PWR->SR1, PWR_SR1_WUF_Msk);
	SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
	__WFI();
}
