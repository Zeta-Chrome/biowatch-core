#include "hal/rtc/rtc.h"
#include "assert.h"
#include "exti/exti.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

#define NULL ((void *)0)

static wkup_callback_t wkup_callback = NULL;
static alrm_callback_t alrm_callback = NULL;

static void rtc_unlock()
{
    // Disable the write protection for RTC
    reg_set_mask(&PWR->CR1, PWR_CR1_DBP_Msk);

    // Unlock the write protections by writing keys
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
}

static void rtc_lock()
{
    // Lock the RTC
    reg_set_field(&RTC->WPR, RTC_WPR_KEY_Pos, 8, 0x00);

    // Enable write protection
    reg_clear_mask(&PWR->CR1, PWR_CR1_DBP_Msk);
}

static void rtc_clock_init()
{
    rtc_unlock();

    // Enable LSE clock
    reg_set_mask(&RCC->BDCR, RCC_BDCR_LSEON_Msk);
    while (reg_get_bit(&RCC->BDCR, RCC_BDCR_LSERDY_Pos) != 1);

    reg_set_field(&RCC->BDCR, RCC_BDCR_RTCSEL_Pos, 2, 0x01);  // Use LSE clock
    reg_set_mask(&RCC->BDCR, RCC_BDCR_RTCEN_Msk);
    reg_set_mask(&RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN_Msk);
}

static void rtc_enable_init()
{
    rtc_unlock();

    // Calendar intialization
    reg_set_mask(&RTC->ISR, RTC_ISR_INIT_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_INITF_Pos) != 1);
}

static void rtc_disable_init()
{
    // Exit initalization
    reg_clear_mask(&RTC->ISR, RTC_ISR_INIT_Msk);

    rtc_lock();
}

void hal_rtc_init()
{
    hal_rtc_disable_alarm();
    hal_rtc_disable_wut();

    // Initialize the RTC clock
    rtc_clock_init();

    rtc_enable_init();
    // prescalers are default values
    // Set to 00:00 1st Jan 2026
    reg_set_field(&RTC->CR, RTC_CR_FMT_Pos, 1, RTC_HR_FMT_24);
    RTC->TR = 0x0; // from 00:00 midnight
    
    uint32_t dr = 0x0UL;
    reg_set_field(&dr, RTC_DR_YT_Pos, 4, 0x2);
    reg_set_field(&dr, RTC_DR_YU_Pos, 4, 0x6);
    reg_set_field(&dr, RTC_DR_MT_Pos, 1, 0x0);
    reg_set_field(&dr, RTC_DR_MU_Pos, 4, 0x1);
    reg_set_field(&dr, RTC_DR_WDU_Pos, 3, 0x6);  // Saturday
    reg_set_field(&dr, RTC_DR_DT_Pos, 2, 0x0);
    reg_set_field(&dr, RTC_DR_DU_Pos, 4, 0x1);

    RTC->DR = dr;

    rtc_disable_init();
}

void hal_rtc_set_hr_format(rtc_hr_fmt_t hr_fmt)
{
    rtc_enable_init();
    reg_set_field(&RTC->CR, RTC_CR_FMT_Pos, 1, hr_fmt);
    rtc_disable_init();
}

void hal_rtc_get_time(uint8_t *hr, uint8_t *min, uint8_t *sec, bool *pm)
{
    // clear RSF then wait for shadow registers to sync
    reg_clear_mask(&RTC->ISR, RTC_ISR_RSF_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_RSF_Pos) != 1);

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
}

void hal_rtc_set_time(uint8_t hr, uint8_t min, uint8_t sec, bool pm)
{
    BW_ASSERT(min < 60, "Invalid minute %d (Expected 0-59)", min);
    BW_ASSERT(sec < 60, "Invalid second %d (Expected 0-59)", sec);
    rtc_enable_init();

    if (reg_get_bit(&RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12)
    {
        BW_ASSERT(hr < 12, "Invalid hour %d (Expected 0-11)", hr);
        reg_set_field(&RTC->TR, RTC_TR_PM_Pos, 1, pm);
    }
    else
    {
        BW_ASSERT(hr < 24, "Invalid hour %d (Expected 0-23)", hr);
    }

    uint32_t tr = 0x0UL;
    reg_set_field(&tr, RTC_TR_HT_Pos, 2, (hr / 10) % 10);
    reg_set_field(&tr, RTC_TR_HU_Pos, 4, hr % 10);
    reg_set_field(&tr, RTC_TR_MNT_Pos, 3, (min / 10) % 10);
    reg_set_field(&tr, RTC_TR_MNU_Pos, 4, min % 10);
    reg_set_field(&tr, RTC_TR_ST_Pos, 3, (sec / 10) % 10);
    reg_set_field(&tr, RTC_TR_SU_Pos, 4, sec % 10);

    RTC->TR = tr;

    rtc_disable_init();
}

void hal_rtc_get_date(uint8_t *yr, uint8_t *mth, uint8_t *dte, uint8_t *wd)
{
    // clear RSF then wait for shadow registers to sync
    reg_clear_mask(&RTC->ISR, RTC_ISR_RSF_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_RSF_Pos) != 1);

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
}

void hal_rtc_set_date(uint8_t yr, uint8_t mth, uint8_t dte, uint8_t wd)
{
    BW_ASSERT(yr < 99, "Invalid year %d (Expected 0-59)", yr);
    BW_ASSERT(mth <= 12 && mth >= 1, "Invalid month %d (Expected 1-12)", mth);
    BW_ASSERT(dte <= 31 && dte >= 1, "Invalid date %d (Expected 1-31)", dte);
    BW_ASSERT(wd <= 7 && wd >= 1, "Invalid weekday %d (Expected 1-12)", wd);
    rtc_enable_init();

    uint32_t dr = 0x0UL;
    reg_set_field(&dr, RTC_DR_YT_Pos, 4, (yr / 10) % 10);
    reg_set_field(&dr, RTC_DR_YU_Pos, 4, yr % 10);
    reg_set_field(&dr, RTC_DR_WDU_Pos, 4, wd % 10);
    reg_set_field(&dr, RTC_DR_MT_Pos, 1, (mth / 10) % 10);
    reg_set_field(&dr, RTC_DR_MU_Pos, 4, mth % 10);
    reg_set_field(&dr, RTC_DR_DT_Pos, 2, (dte / 10) % 10);
    reg_set_field(&dr, RTC_DR_DU_Pos, 4, dte % 10);
    RTC->DR = dr;

    rtc_disable_init();
}

void hal_rtc_wkup_isr()
{
    rtc_unlock();
    reg_clear_mask(&RTC->ISR, RTC_ISR_WUTF_Msk);
    rtc_lock();

    if (wkup_callback != NULL)
    {
        wkup_callback();
    }
}

void hal_rtc_enable_wut(uint8_t delay_s, wkup_callback_t callback, uint8_t irq_priority)
{
    rtc_unlock();

    // Disable wake-up timer
    reg_clear_mask(&RTC->CR, RTC_CR_WUTE_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_WUTWF_Pos) != 1);

    // Configure the value
    RTC->WUTR = delay_s - 1;
    reg_set_field(&RTC->CR, RTC_CR_WUCKSEL_Pos, 3, 0x4);

    // Enable wake-up timer interrupt
    reg_set_mask(&RTC->CR, RTC_CR_WUTIE_Msk);

    exti_conf_t exti_conf = {.im = WKUP_IM,
                             .edge = EXTI_EDGE_RISING,
                             .irq = RTC_WKUP_IRQn,
                             .irq_priority = irq_priority,
                             .on_interrupt = hal_rtc_wkup_isr};
    hal_exti_init(&exti_conf);

    // Enable wake-up timer
    reg_set_mask(&RTC->CR, RTC_CR_WUTE_Msk);
    wkup_callback = callback;

    rtc_lock();
}

void hal_rtc_disable_wut()
{
    rtc_unlock();
    hal_exti_deinit(WKUP_IM);
    reg_clear_mask(&RTC->ISR, RTC_ISR_WUTF_Msk);
    reg_clear_mask(&RTC->CR, RTC_CR_WUTIE_Msk);

    // disable wake-up timer
    reg_clear_mask(&RTC->CR, RTC_CR_WUTE_Msk);
    while (reg_get_bit(&RTC->ISR, RTC_ISR_WUTWF_Pos) != 1);

    rtc_lock();
}

void hal_rtc_alrm_isr()
{
    rtc_unlock();
    reg_clear_mask(&RTC->ISR, RTC_ISR_ALRAF_Msk);
    rtc_lock();

    if (alrm_callback != NULL)
    {
        alrm_callback();
    }
}

void hal_rtc_enable_alarm(uint8_t hr, uint8_t min, uint8_t sec, bool pm, alrm_callback_t callback,
                          uint8_t irq_priority)
{
    rtc_unlock();

    BW_ASSERT(min < 60, "Invalid minute %d (Expected 0-59)", min);
    BW_ASSERT(sec < 60, "Invalid second %d (Expected 0-59)", sec);

    reg_clear_mask(&RTC->CR, RTC_CR_ALRAE_Msk);
    while(reg_get_bit(&RTC->ISR, RTC_ISR_ALRAWF_Pos) != 1);

    uint32_t alrmar = 0x0UL;
    reg_set_mask(&alrmar, RTC_ALRMAR_MSK4_Msk);  // Don't care date/day
    if (reg_get_bit(&RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12)
    {
        BW_ASSERT(hr < 12, "Invalid hour %d (Expected 0-11)", hr);
        reg_set_field(&alrmar, RTC_ALRMAR_PM_Pos, 1, pm);
    }
    else
    {
        BW_ASSERT(hr < 24, "Invalid hour %d (Expected 0-23)", hr);
        reg_clear_mask(&alrmar, RTC_ALRMAR_PM_Msk);
    }

    reg_set_field(&alrmar, RTC_ALRMAR_HT_Pos, 2, (hr / 10) % 10);
    reg_set_field(&alrmar, RTC_ALRMAR_HU_Pos, 4, hr % 10);
    reg_set_field(&alrmar, RTC_ALRMAR_MNT_Pos, 3, (min / 10) % 10);
    reg_set_field(&alrmar, RTC_ALRMAR_MNU_Pos, 4, min % 10);
    reg_set_field(&alrmar, RTC_ALRMAR_ST_Pos, 3, (sec / 10) % 10);
    reg_set_field(&alrmar, RTC_ALRMAR_SU_Pos, 4, sec % 10);
    RTC->ALRMAR = alrmar;

    reg_set_mask(&RTC->CR, RTC_CR_ALRAIE_Msk);

    exti_conf_t exti_conf = {.im = ALRM_IM,
                             .edge = EXTI_EDGE_RISING,
                             .irq = RTC_Alarm_IRQn,
                             .irq_priority = irq_priority,
                             .on_interrupt = hal_rtc_alrm_isr};
    hal_exti_init(&exti_conf);

    reg_set_mask(&RTC->CR, RTC_CR_ALRAE_Msk);
    alrm_callback = callback;

    rtc_lock();
}

void hal_rtc_disable_alarm()
{
    rtc_unlock();
    hal_exti_deinit(ALRM_IM);
    reg_clear_mask(&RTC->ISR, RTC_ISR_ALRAF_Msk);
    reg_clear_mask(&RTC->CR, RTC_CR_ALRAIE_Msk);
    reg_clear_mask(&RTC->CR, RTC_CR_ALRAE_Msk);
    rtc_lock();
}
