#include "drivers/clock/clock.h"
#include "drivers/clock/clock_srcs.h"
#include "drivers/exti/exti.h"
#include "lib/utils.h"
#include "pwr.h"
#include "stm32wb55xx.h"

#define CPU2_SEV_IM 41

static exti_handle_t g_exti_h;

void pwr_enable_wkup(pwr_wkup_t wkup, pwr_wkup_edge_t edge)
{
    SET_FIELD(PWR->CR3, PWR_CR3_EWUP1_Msk << wkup);
    MODIFY_FIELD_W(PWR->CR4, 1, PWR_CR4_WP1_Pos + wkup, edge);
}

void pwr_boot_cpu2()
{
    // Enable hse for ble
    clock_enable_hse();

    SET_FIELD(RCC->C2AHB3ENR, RCC_C2AHB3ENR_IPCCEN_Msk);
    __IO uint32_t tempreg = RCC->C2AHB3ENR & RCC_C2AHB3ENR_IPCCEN_Msk;
    (void)tempreg;

    // Enable SEV EXTI for cpu2 wakeup
    exti_conf_t conf = {.im = CPU2_SEV_IM, .edge = EXTI_EDGE_RISING, .irq_priority = 0, .callback = NULL, .user_data = NULL};
    exti_init(&conf, &g_exti_h);

    __SEV(); // Send Event
    __WFE(); // Clear event flag

    // Boot CPU2
    SET_FIELD(PWR->CR4, PWR_CR4_C2BOOT_Msk);
}

void pwr_enter_sleep()
{
    CLEAR_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void pwr_enter_lp_sleep()
{
    // Configure clock frequence below 2MHz
    clock_conf_t conf = clock_conf_lp_sleep();
    clock_reconfigure(&conf);

    // Set LPR
    SET_FIELD(PWR->CR1, PWR_CR1_LPR_Msk);

    // Enter sleep
    pwr_enter_sleep();
}

void pwr_exit_lp_sleep()
{
    // Clear LPR
    CLEAR_FIELD(PWR->CR1, PWR_CR1_LPR_Msk);
    while (PWR->SR2 & PWR_SR2_REGLPF_Msk);

    // Increase clock
    clock_conf_t conf = clock_conf_performance();
    clock_reconfigure(&conf);
}

void pwr_enter_stop(pwr_stop_mode_t stop_mode)
{
    // Configure clock to MSI 48Mhz
    clock_conf_t conf = clock_conf_stop();
    clock_reconfigure(&conf);

    MODIFY_FIELD(PWR->CR1, PWR_CR1_LPMS_Msk, PWR_CR1_LPMS_Pos, stop_mode);
    SET_FIELD(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void pwr_exit_stop()
{
    clock_conf_t conf = clock_conf_performance();
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
