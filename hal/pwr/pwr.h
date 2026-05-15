#ifndef HAL_PWR_H
#define HAL_PWR_H

typedef enum 
{
    PWR_STOP_MODE_STOP0,
    PWR_STOP_MODE_STOP1,
    PWR_STOP_MODE_STOP2,
} pwr_stop_mode_t;

void hal_pwr_enter_sleep();

void hal_pwr_enter_lp_sleep();
void hal_pwr_exit_lp_sleep();

void hal_pwr_enter_stop(pwr_stop_mode_t stop_mode);
void hal_pwr_exit_stop();

void hal_pwr_enter_standby();

void hal_pwr_enter_shutdown();

#endif
