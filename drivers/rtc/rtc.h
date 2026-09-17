#ifndef DRIVER_RTC_H
#define DRIVER_RTC_H

#include "stm32wb55xx.h"
#include <stdbool.h>
#include <stdint.h>

#define WKUP_IM 19
#define ALRM_IM 17

typedef void (*wkup_callback_t)(uint16_t seconds);
typedef void (*alrm_callback_t)(void);

enum rtc_hr_fmt { RTC_HR_FMT_24, RTC_HR_FMT_12 };
enum rtc_alarm_mask {
	RTC_ALARM_MASK_HOUR = RTC_ALRMAR_MSK3_Msk,
	RTC_ALARM_MASK_MIN = RTC_ALRMAR_MSK2_Msk,
	RTC_ALARM_MASK_SEC = RTC_ALRMAR_MSK1_Msk
};

void rtc_init();
void rtc_set_hr_format(enum rtc_hr_fmt hr_fmt);
void rtc_get_time(uint8_t *hr, uint8_t *min, uint8_t *sec, bool *pm);
void rtc_set_time(uint8_t hr, uint8_t min, uint8_t sec, bool pm);
void rtc_get_date(uint8_t *yr, uint8_t *mth, uint8_t *dte, uint8_t *wd);
void rtc_set_date(uint8_t yr, uint8_t mth, uint8_t dte, uint8_t wd);
uint32_t rtc_get_timestamp();
uint32_t rtc_get_time_ms();
void rtc_enable_wut(uint16_t delay_s, wkup_callback_t callback, uint8_t irq_priority);
void rtc_disable_wut();
void rtc_enable_alarm(uint8_t hr, uint8_t min, uint8_t sec, bool pm, uint32_t mask,
					  alrm_callback_t callback, uint8_t irq_priority);
void rtc_disable_alarm();

#endif
