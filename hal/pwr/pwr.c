#include "pwr.h"
#include "clock/clock.h"
#include "clock/clock_srcs.h"
#include "exti/exti.h"
#include "reg.h"
#include "stm32wb55xx.h"
#include "utils.h"

#define CPU2_SEV_IM 41

static exti_handle_t g_exti_h;

void hal_pwr_enable_wkup(pwr_wkup_t wkup, pwr_wkup_edge_t edge)
{
    reg_set_mask(&PWR->CR3, PWR_CR3_EWUP1_Msk << wkup);
    reg_set_field(&PWR->CR4, PWR_CR4_WP1_Pos + wkup, 1, edge);
}

void hal_pwr_boot_cpu2()
{
    // Enable hse for ble 
    hal_clock_enable_hse();

    // Enable SEV EXTI for cpu2 wakeup
    exti_conf_t conf = {.im = CPU2_SEV_IM, .edge = EXTI_EDGE_RISING, .irq_priority = 0, .callback = NULL, .user_data = NULL};
    hal_exti_init(&conf, &g_exti_h);
 
    __SEV(); // Send Event
    __WFE(); // Clear event flag

    // Boot CPU2
    reg_set_mask(&PWR->CR4, PWR_CR4_C2BOOT_Msk); 
}

void hal_pwr_enter_sleep()
{
    reg_clear_mask(&SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void hal_pwr_enter_lp_sleep()
{
    // Configure clock frequence below 2MHz
    clock_conf_t conf = clock_conf_lp_sleep();
    hal_clock_reconfigure(&conf);

    // Set LPR
    reg_set_mask(&PWR->CR1, PWR_CR1_LPR_Msk);

    // Enter sleep
    hal_pwr_enter_sleep();
}

void hal_pwr_exit_lp_sleep()
{
    // Clear LPR
    reg_clear_mask(&PWR->CR1, PWR_CR1_LPR_Msk);
    while (PWR->SR2 & PWR_SR2_REGLPF_Msk);

    // Increase clock
    clock_conf_t conf = clock_conf_performance();
    hal_clock_reconfigure(&conf);
}

void hal_pwr_enter_stop(pwr_stop_mode_t stop_mode)
{
    // Configure clock to MSI 48Mhz 
    clock_conf_t conf = clock_conf_stop();
    hal_clock_reconfigure(&conf);

    reg_set_field(&PWR->CR1, PWR_CR1_LPMS_Pos, 3, stop_mode);
    reg_set_mask(&SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void hal_pwr_exit_stop() 
{
    clock_conf_t conf = clock_conf_performance();
    hal_clock_reconfigure(&conf);
}

void hal_pwr_enter_standby() 
{
    reg_set_field(&PWR->CR1, PWR_CR1_LPMS_Pos, 3, 0x3);
    reg_clear_mask(&PWR->SR1, PWR_SR1_WUF_Msk);
    reg_set_mask(&SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void hal_pwr_enter_shutdown() 
{
    reg_set_field(&PWR->CR1, PWR_CR1_LPMS_Pos, 3, 0x4);
    reg_clear_mask(&PWR->SR1, PWR_SR1_WUF_Msk);
    reg_set_mask(&SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}
