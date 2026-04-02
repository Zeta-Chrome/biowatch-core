#include "hal/rtc/rtc.h"
#include "assert.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

#define NULL ((void *)0)

static wkup_callback_t wkup_callback = NULL;
static alrm_callback_t alrm_callback = NULL;

static void rtc_clock_init()
{
    reg_set_field(&RCC->BDCR, RCC_BDCR_RTCSEL_Pos, 2, 0x01);  // Use LSE clock
    reg_set_mask(&RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN_Msk);
}

static void rtc_disable_cfg()
{
    // Disable the write protection for RTC
    reg_set_mask(&PWR->CR1, PWR_CR1_DBP_Msk);

    // Unlock the write protections by writing keys
    reg_set_field(&RTC->WPR, RTC_WPR_KEY_Pos, 8, 0xCA);  // Key 1
    reg_set_field(&RTC->WPR, RTC_WPR_KEY_Pos, 8, 0x53);  // Key 2

    // Calendar intialization
    reg_set_mask(&RTC->ISR, RTC_ISR_INIT_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_INITF_Pos) != 1);
}

static void rtc_enable_cfg()
{
    // Exit initalization
    reg_clear_mask(&RTC->ISR, RTC_ISR_INIT_Msk);

    // Lock the RTC
    reg_set_field(&RTC->WPR, RTC_WPR_KEY_Pos, 8, 0x00);

    // Enable write protection
    reg_clear_mask(&PWR->CR1, PWR_CR1_DBP_Msk);
}

void hal_rtc_init()
{
    // Initialize the RTC clock
    rtc_clock_init();

    rtc_enable_cfg();
    // prescalers are default values
    // Set to 00:00 1st Jan 2026
    reg_set_field(&RTC->CR, RTC_CR_FMT_Pos, 1, RTC_HR_FMT_24);
    reg_clear_mask(&RTC->TR, RTC_TR_SU_Msk | RTC_TR_ST_Msk | RTC_TR_MNU_Msk | RTC_TR_MNT_Msk |
                             RTC_TR_HU_Pos | RTC_TR_HT_Msk | RTC_TR_PM_Msk);  // from 00:00 midnight
    reg_set_field(&RTC->DR, RTC_DR_YT_Pos, 4, 0x2);
    reg_set_field(&RTC->DR, RTC_DR_YU_Pos, 4, 0x6);
    reg_set_field(&RTC->DR, RTC_DR_MT_Pos, 1, 0x0);
    reg_set_field(&RTC->DR, RTC_DR_MU_Pos, 4, 0x1);
    reg_set_field(&RTC->DR, RTC_DR_WDU_Pos, 3, 0xE);  // Saturday
    reg_set_field(&RTC->DR, RTC_DR_DT_Pos, 2, 0x0);
    reg_set_field(&RTC->DR, RTC_DR_DU_Pos, 4, 0x1);

    rtc_disable_cfg();
}

void hal_rtc_set_hr_format(rtc_hr_fmt_t hr_fmt)
{
    rtc_enable_cfg();
    reg_set_field(&RTC->CR, RTC_CR_FMT_Pos, 1, hr_fmt);
    rtc_disable_cfg();
}

void hal_rtc_get_time(uint8_t *hr, uint8_t *min, uint8_t *sec, bool *pm)
{
    rtc_enable_cfg();
    *pm = 0;
    if (reg_get_bit(&RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12)
    {
        *pm = reg_get_bit(&RTC->TR, RTC_TR_PM_Pos);
    }

    if (hr)
    {
        uint8_t ht = reg_get_field(&RTC->TR, RTC_TR_HT_Pos, 2);
        uint8_t hu = reg_get_field(&RTC->TR, RTC_TR_HU_Pos, 4);
        *hr = ht * 10 + hu;
    }

    if (min)
    {
        uint8_t mt = reg_get_field(&RTC->TR, RTC_TR_MNT_Pos, 3);
        uint8_t mu = reg_get_field(&RTC->TR, RTC_TR_MNU_Pos, 4);
        *min = mt * 10 + mu;
    }

    if (sec)
    {
        uint8_t st = reg_get_field(&RTC->TR, RTC_TR_ST_Pos, 3);
        uint8_t su = reg_get_field(&RTC->TR, RTC_TR_SU_Pos, 4);
        *sec = st * 10 + su;
    }

    rtc_disable_cfg();
}

void hal_rtc_set_time(uint8_t hr, uint8_t min, uint8_t sec, bool pm)
{
    BW_ASSERT(min < 60, "Invalid minute %d (Expected 0-59)", min);
    BW_ASSERT(sec < 60, "Invalid second %d (Expected 0-59)", sec);
    rtc_enable_cfg();

    if (reg_get_bit(&RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12)
    {
        BW_ASSERT(hr < 12, "Invalid hour %d (Expected 0-11)", hr);
        reg_set_field(&RTC->TR, RTC_TR_PM_Pos, 1, pm);
    }
    else
    {
        BW_ASSERT(hr < 24, "Invalid hour %d (Expected 0-23)", hr);
    }
    reg_set_field(&RTC->TR, RTC_TR_HT_Pos, 2, (hr / 10) % 10);
    reg_set_field(&RTC->TR, RTC_TR_HU_Pos, 4, hr % 10);
    reg_set_field(&RTC->TR, RTC_TR_MNT_Pos, 3, (min / 10) % 10);
    reg_set_field(&RTC->TR, RTC_TR_MNU_Pos, 4, min % 10);
    reg_set_field(&RTC->TR, RTC_TR_ST_Pos, 3, (sec / 10) % 10);
    reg_set_field(&RTC->TR, RTC_TR_SU_Pos, 4, sec % 10);

    rtc_disable_cfg();
}

void hal_rtc_get_date(uint8_t *yr, uint8_t *mth, uint8_t *dte, uint8_t *wd)
{
    rtc_enable_cfg();

    if (yr)
    {
        uint8_t yt = reg_get_field(&RTC->DR, RTC_DR_YT_Pos, 4);
        uint8_t yu = reg_get_field(&RTC->DR, RTC_DR_YU_Pos, 4);
        *yr = yt * 10 + yu;
    }

    if (wd)
    {
        *wd = reg_get_field(&RTC->DR, RTC_DR_WDU_Pos, 4);
    }

    if (mth)
    {
        uint8_t mt = reg_get_field(&RTC->DR, RTC_DR_MT_Pos, 1);
        uint8_t mu = reg_get_field(&RTC->DR, RTC_DR_MU_Pos, 4);
        *mth = mt * 10 + mu;
    }

    if (dte)
    {
        uint8_t dt = reg_get_field(&RTC->DR, RTC_DR_DT_Pos, 2);
        uint8_t du = reg_get_field(&RTC->DR, RTC_DR_DU_Pos, 4);
        *dte = dt * 10 + du;
    }

    rtc_disable_cfg();
}

void hal_rtc_set_date(uint8_t yr, uint8_t mth, uint8_t dte, uint8_t wd)
{
    BW_ASSERT(yr < 99, "Invalid year %d (Expected 0-59)", yr);
    BW_ASSERT(mth <= 12 && mth >= 1, "Invalid month %d (Expected 1-12)", mth);
    BW_ASSERT(dte <= 31 && dte >= 1, "Invalid date %d (Expected 1-31)", dte);
    BW_ASSERT(wd <= 7 && wd >= 1, "Invalid weekday %d (Expected 1-12)", wd);
    rtc_enable_cfg();

    reg_set_field(&RTC->DR, RTC_DR_YT_Pos, 4, (yr / 10) % 10);
    reg_set_field(&RTC->DR, RTC_DR_YU_Pos, 4, yr % 10);
    reg_set_field(&RTC->DR, RTC_DR_WDU_Pos, 4, wd % 10);
    reg_set_field(&RTC->DR, RTC_DR_MT_Pos, 1, (mth / 10) % 10);
    reg_set_field(&RTC->DR, RTC_DR_MU_Pos, 4, mth % 10);
    reg_set_field(&RTC->DR, RTC_DR_DT_Pos, 2, (dte / 10) % 10);
    reg_set_field(&RTC->DR, RTC_DR_DU_Pos, 4, dte % 10);

    rtc_disable_cfg();
}

void hal_rtc_wkup_isr()
{
    if (wkup_callback != NULL)
    {
        wkup_callback();
    }
}

void hal_rtc_enable_wut(uint8_t delay_s, wkup_callback_t callback, uint8_t irq_priority)
{
    rtc_enable_cfg();

    // Disable wake-up timer
    reg_clear_mask(&RTC->CR, RTC_CR_WUTE_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_WUTF_Pos) != 1);

    // Configure the value
    reg_set_field(&RTC->WUTR, RTC_WUTR_WUT_Pos, 16, delay_s - 1);
    reg_set_field(&RTC->CR, RTC_CR_WUCKSEL_Pos, 3, 0x4);

    // Enable wake-up timer interrupt
    reg_set_mask(&RTC->CR, RTC_CR_WUTIE_Msk);

    NVIC_EnableIRQ(RTC_WKUP_IRQn);
    NVIC_SetPriority(RTC_WKUP_IRQn, irq_priority);

    // Enable wake-up timer
    reg_set_mask(&RTC->CR, RTC_CR_WUTE_Msk);
    wkup_callback = callback;

    rtc_disable_cfg();
}

void hal_rtc_disable_wut()
{
    rtc_enable_cfg();

    // disable wake-up timer interrupt
    NVIC_DisableIRQ(RTC_WKUP_IRQn);
    reg_clear_mask(&RTC->CR, RTC_CR_WUTIE_Msk);

    // disable wake-up timer
    reg_clear_mask(&RTC->CR, RTC_CR_WUTE_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_WUTF_Pos) != 1);

    rtc_disable_cfg();
}

void hal_rtc_alrm_isr()
{
    if (alrm_callback != NULL)
    {
        alrm_callback();
    }
}

void hal_rtc_enable_alarm(uint8_t hr, uint8_t min, uint8_t sec, bool pm, alrm_callback_t callback,
                          uint8_t irq_priority)
{
    rtc_enable_cfg();

    reg_clear_mask(&RTC->CR, RTC_CR_ALRAE_Msk);

    reg_set_mask(&RTC->ALRMAR, RTC_ALRMAR_MSK4_Msk);  // Don't care date/day
    if (reg_get_bit(&RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12)
    {
        reg_set_field(&RTC->ALRMAR, RTC_ALRMAR_PM_Pos, 1, pm);
    }
    else
    {
        reg_clear_mask(&RTC->ALRMAR, RTC_ALRMAR_PM_Msk);
    }
    reg_set_field(&RTC->ALRMAR, RTC_ALRMAR_HT_Pos, 2, (hr / 10) % 10);
    reg_set_field(&RTC->ALRMAR, RTC_ALRMAR_HU_Pos, 2, hr % 10);
    reg_set_field(&RTC->ALRMAR, RTC_ALRMAR_MNT_Pos, 3, (min / 10) % 10);
    reg_set_field(&RTC->TR, RTC_ALRMAR_MNU_Pos, 4, min % 10);
    reg_set_field(&RTC->TR, RTC_ALRMAR_ST_Pos, 3, (sec / 10) % 10);
    reg_set_field(&RTC->TR, RTC_ALRMAR_SU_Pos, 4, sec % 10);

    reg_set_mask(&RTC->CR, RTC_CR_ALRAIE_Msk);
    NVIC_EnableIRQ(RTC_Alarm_IRQn);
    NVIC_SetPriority(RTC_Alarm_IRQn, irq_priority);

    reg_set_mask(&RTC->CR, RTC_CR_ALRAE_Msk);
    alrm_callback = callback;

    rtc_disable_cfg();
}

void hal_rtc_disable_alarm()
{
    rtc_enable_cfg();

    NVIC_DisableIRQ(RTC_Alarm_IRQn);
    reg_clear_mask(&RTC->CR, RTC_CR_ALRAIE_Msk);
    reg_clear_mask(&RTC->CR, RTC_CR_ALRAE_Msk);

    rtc_enable_cfg();
}
