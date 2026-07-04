#ifndef HAL_PWR_H
#define HAL_PWR_H

typedef enum
{
    PWR_STOP_MODE_STOP0,
    PWR_STOP_MODE_STOP1,
    PWR_STOP_MODE_STOP2,
} pwr_stop_mode_t;

typedef enum
{
    PWR_WKUP1,
    PWR_WKUP2,
    PWR_WKUP3,
    PWR_WKUP4,
    PWR_WKUP5,
} pwr_wkup_t;

typedef enum
{
    PWR_WKUP_EDGE_RISING,
    PWR_WKUP_EDGE_FALLING,
} pwr_wkup_edge_t;

void hal_pwr_enable_wkup(pwr_wkup_t wkup, pwr_wkup_edge_t edge);
void hal_pwr_boot_cpu2();
void hal_pwr_enter_sleep();
void hal_pwr_enter_lp_sleep();
void hal_pwr_exit_lp_sleep();
void hal_pwr_enter_stop(pwr_stop_mode_t stop_mode);
void hal_pwr_exit_stop();
void hal_pwr_enter_standby();
void hal_pwr_enter_shutdown();

#endif
