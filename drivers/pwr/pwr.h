#ifndef DRIVER_PWR_H
#define DRIVER_PWR_H

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

void pwr_enable_wkup(pwr_wkup_t wkup, pwr_wkup_edge_t edge);
void pwr_boot_cpu2();
void pwr_enter_sleep();
void pwr_enter_lp_sleep();
void pwr_exit_lp_sleep();
void pwr_enter_stop(pwr_stop_mode_t stop_mode);
void pwr_exit_stop();
void pwr_enter_standby();
void pwr_enter_shutdown();

#endif
