#ifndef DRIVER_PWR_H
#define DRIVER_PWR_H

enum pwr_stop_mode {
	PWR_STOP_MODE_STOP0,
	PWR_STOP_MODE_STOP1,
	PWR_STOP_MODE_STOP2,
};

enum pwr_wkup {
	PWR_WKUP1,
	PWR_WKUP2,
	PWR_WKUP3,
	PWR_WKUP4,
	PWR_WKUP5,
};

enum pwr_wkup_edge {
	PWR_WKUP_EDGE_RISING,
	PWR_WKUP_EDGE_FALLING,
};

void pwr_enable_wkup(enum pwr_wkup wkup, enum pwr_wkup_edge edge);
void pwr_boot_cpu2();
void pwr_enter_sleep();
void pwr_enter_lp_sleep();
void pwr_exit_lp_sleep();
void pwr_enter_stop(enum pwr_stop_mode stop_mode);
void pwr_exit_stop();
void pwr_enter_standby();
void pwr_enter_shutdown();

#endif
