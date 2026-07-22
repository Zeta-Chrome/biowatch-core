#ifndef DRIVER_RTC_H
#define DRIVER_RTC_H

#include <stdbool.h>
#include <stdint.h>

#define WKUP_IM 19
#define ALRM_IM 17

typedef void (*wkup_callback_t)(void);
typedef void (*alrm_callback_t)(void);

enum rtc_hr_fmt { RTC_HR_FMT_24, RTC_HR_FMT_12 };

void rtc_init();
void rtc_set_hr_format(enum rtc_hr_fmt hr_fmt);
void rtc_get_time(uint8_t *hr, uint8_t *min, uint8_t *sec, bool *pm);
void rtc_set_time(uint8_t hr, uint8_t min, uint8_t sec, bool pm);
void rtc_get_date(uint8_t *yr, uint8_t *mth, uint8_t *dte, uint8_t *wd);
void rtc_set_date(uint8_t yr, uint8_t mth, uint8_t dte, uint8_t wd);
void rtc_enable_wut(uint8_t delay_s, wkup_callback_t callback, uint8_t irq_priority);
void rtc_disable_wut();
void rtc_enable_alarm(uint8_t hr, uint8_t min, uint8_t sec, bool pm, alrm_callback_t callback,
					  uint8_t irq_priority);
void rtc_disable_alarm();

#endif
