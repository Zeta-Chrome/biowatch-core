#ifndef HAL_RTC_H
#define HAL_RTC_H

#include <stdint.h>

typedef enum 
{
    RTC_HR_FMT_24,
    RTC_HR_FMT_12
} rtc_hr_fmt_t;

void hal_rtc_init();
void hal_rtc_set_hr_format(rtc_hr_fmt_t hr_fmt);

#endif
