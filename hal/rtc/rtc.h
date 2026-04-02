#ifndef HAL_RTC_H
#define HAL_RTC_H

#include <stdbool.h>
#include <stdint.h>

#define WKUP_IM 19
#define ALRM_IM 17

typedef void (*wkup_callback_t)(void);
typedef void (*alrm_callback_t)(void);

typedef enum 
{
    RTC_HR_FMT_24,
    RTC_HR_FMT_12
} rtc_hr_fmt_t;

void hal_rtc_init();
void hal_rtc_set_hr_format(rtc_hr_fmt_t hr_fmt);
void hal_rtc_get_time(uint8_t *hr, uint8_t *min, uint8_t *sec, bool *pm);
void hal_rtc_set_time(uint8_t hr, uint8_t min, uint8_t sec, bool pm);
void hal_rtc_get_date(uint8_t *yr, uint8_t *mth, uint8_t *dte, uint8_t *wd);
void hal_rtc_set_date(uint8_t yr, uint8_t mth, uint8_t dte, uint8_t wd);
void hal_rtc_wkup_isr();
void hal_rtc_enable_wut(uint8_t delay_s, wkup_callback_t callback, uint8_t irq_priority);
void hal_rtc_disable_wut();
void hal_rtc_alrm_isr();
void hal_rtc_enable_alarm(uint8_t hr, uint8_t min, uint8_t sec, bool pm, alrm_callback_t callback, uint8_t irq_priority);
void hal_rtc_disable_alarm();

#endif
