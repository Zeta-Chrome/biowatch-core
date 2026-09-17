#include "drivers/clock/clock_srcs.h"
#include "drivers/exti/exti.h"
#include "drivers/pwr/pwr.h"
#include "lib/assert.h"
#include "lib/utils.h"
#include "rtc.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

#define NULL ((void *)0)

static struct exti_handle g_wkup_exti_h;
static struct exti_handle g_alrm_exti_h;
static wkup_callback_t g_wkup_callback = NULL;
static alrm_callback_t g_alrm_callback = NULL;

static inline void rtc_unlock()
{
	pwr_unlock_backup_domain();
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
}

static inline void rtc_lock()
{
	MODIFY_FIELD(RTC->WPR, RTC_WPR_KEY_Msk, RTC_WPR_KEY_Pos, 0x00);
	pwr_lock_backup_domain();
}

static void rtc_clock_init()
{
	// Enable LSE clock
	clock_enable_lse();

	rtc_unlock();

	MODIFY_FIELD(RCC->BDCR, RCC_BDCR_RTCSEL_Msk, RCC_BDCR_RTCSEL_Pos, 0x01); // Use LSE clock
	SET_FIELD(RCC->BDCR, RCC_BDCR_RTCEN_Msk);
	SET_FIELD(RCC->APB1ENR1, RCC_APB1ENR1_RTCAPBEN_Msk);
}

static void rtc_enable_init()
{
	rtc_unlock();

	// Calendar intialization
	SET_FIELD(RTC->ISR, RTC_ISR_INIT_Msk);
	while (!(RTC->ISR & RTC_ISR_INITF_Msk))
		;
}

static void rtc_disable_init()
{
	CLEAR_FIELD(RTC->ISR, RTC_ISR_INIT_Msk);
	rtc_lock();
}

static inline void rtc_clear_isr(uint32_t mask)
{
	WRITE_FIELD(RTC->ISR, ~(mask | RTC_ISR_INIT_Msk));
}

static inline void rtc_sync_shadow_regs()
{
	rtc_clear_isr(RTC_ISR_RSF_Msk);
	while (!(RTC->ISR & RTC_ISR_RSF_Msk))
		;
}

void rtc_init()
{
	rtc_disable_alarm();
	rtc_disable_wut();

	rtc_clock_init();

	rtc_enable_init();
	MODIFY_BIT(RTC->CR, RTC_CR_FMT_Pos, RTC_HR_FMT_24);
	RTC->TR = 0x0; // from 00:00 midnight

	uint32_t dr = 0x0UL;
	MODIFY_FIELD(dr, RTC_DR_YT_Msk, RTC_DR_YT_Pos, 0x2);
	MODIFY_FIELD(dr, RTC_DR_YU_Msk, RTC_DR_YU_Pos, 0x6);
	MODIFY_FIELD(dr, RTC_DR_MT_Msk, RTC_DR_MT_Pos, 0x0);
	MODIFY_FIELD(dr, RTC_DR_MU_Msk, RTC_DR_MU_Pos, 0x1);
	MODIFY_FIELD(dr, RTC_DR_WDU_Msk, RTC_DR_WDU_Pos, 0x6); // Saturday
	MODIFY_FIELD(dr, RTC_DR_DT_Msk, RTC_DR_DT_Pos, 0x0);
	MODIFY_FIELD(dr, RTC_DR_DU_Msk, RTC_DR_DU_Pos, 0x1);

	RTC->DR = dr;

	rtc_disable_init();
}

void rtc_set_hr_format(enum rtc_hr_fmt hr_fmt)
{
	rtc_enable_init();
	MODIFY_BIT(RTC->CR, RTC_CR_FMT_Pos, hr_fmt);
	rtc_disable_init();
}

void rtc_get_time(uint8_t *hr, uint8_t *min, uint8_t *sec, bool *pm)
{
	rtc_sync_shadow_regs();

	uint32_t tr = RTC->TR;

	*pm = 0;
	if (READ_BIT(RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12)
		*pm = (tr & RTC_TR_PM_Msk);

	if (hr) {
		uint8_t ht = READ_FIELD(tr, RTC_TR_HT_Msk, RTC_TR_HT_Pos);
		uint8_t hu = READ_FIELD(tr, RTC_TR_HU_Msk, RTC_TR_HU_Pos);
		*hr = ht * 10 + hu;
	}

	if (min) {
		uint8_t mt = READ_FIELD(tr, RTC_TR_MNT_Msk, RTC_TR_MNT_Pos);
		uint8_t mu = READ_FIELD(tr, RTC_TR_MNU_Msk, RTC_TR_MNU_Pos);
		*min = mt * 10 + mu;
	}

	if (sec) {
		uint8_t st = READ_FIELD(tr, RTC_TR_ST_Msk, RTC_TR_ST_Pos);
		uint8_t su = READ_FIELD(tr, RTC_TR_SU_Msk, RTC_TR_SU_Pos);
		*sec = st * 10 + su;
	}
}

void rtc_set_time(uint8_t hr, uint8_t min, uint8_t sec, bool pm)
{
	BW_ASSERT(min < 60, "Invalid minute %d (Expected 0-59)", min);
	BW_ASSERT(sec < 60, "Invalid second %d (Expected 0-59)", sec);
	rtc_enable_init();

	if (READ_BIT(RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12) {
		BW_ASSERT(hr <= 12, "Invalid hour %d (Expected 0-12)", hr);
		MODIFY_BIT(RTC->TR, RTC_TR_PM_Pos, pm);
	} else {
		BW_ASSERT(hr < 24, "Invalid hour %d (Expected 0-23)", hr);
	}

	uint32_t tr = 0x0UL;
	MODIFY_FIELD(tr, RTC_TR_HT_Msk, RTC_TR_HT_Pos, (hr / 10) % 10);
	MODIFY_FIELD(tr, RTC_TR_HU_Msk, RTC_TR_HU_Pos, hr % 10);
	MODIFY_FIELD(tr, RTC_TR_MNT_Msk, RTC_TR_MNT_Pos, (min / 10) % 10);
	MODIFY_FIELD(tr, RTC_TR_MNU_Msk, RTC_TR_MNU_Pos, min % 10);
	MODIFY_FIELD(tr, RTC_TR_ST_Msk, RTC_TR_ST_Pos, (sec / 10) % 10);
	MODIFY_FIELD(tr, RTC_TR_SU_Msk, RTC_TR_SU_Pos, sec % 10);

	RTC->TR = tr;

	rtc_disable_init();
}

void rtc_get_date(uint8_t *yr, uint8_t *mth, uint8_t *dte, uint8_t *wd)
{
	rtc_sync_shadow_regs();

	uint32_t dr = RTC->DR;

	if (yr) {
		uint8_t yt = READ_FIELD(dr, RTC_DR_YT_Msk, RTC_DR_YT_Pos);
		uint8_t yu = READ_FIELD(dr, RTC_DR_YU_Msk, RTC_DR_YU_Pos);
		*yr = yt * 10 + yu;
	}

	if (wd)
		*wd = READ_FIELD(dr, RTC_DR_WDU_Msk, RTC_DR_WDU_Pos);

	if (mth) {
		uint8_t mt = READ_FIELD(dr, RTC_DR_MT_Msk, RTC_DR_MT_Pos);
		uint8_t mu = READ_FIELD(dr, RTC_DR_MU_Msk, RTC_DR_MU_Pos);
		*mth = mt * 10 + mu;
	}

	if (dte) {
		uint8_t dt = READ_FIELD(dr, RTC_DR_DT_Msk, RTC_DR_DT_Pos);
		uint8_t du = READ_FIELD(dr, RTC_DR_DU_Msk, RTC_DR_DU_Pos);
		*dte = dt * 10 + du;
	}
}

void rtc_set_date(uint8_t yr, uint8_t mth, uint8_t dte, uint8_t wd)
{
	BW_ASSERT(yr < 99, "Invalid year %d (Expected 0-59)", yr);
	BW_ASSERT(mth <= 12 && mth >= 1, "Invalid month %d (Expected 1-12)", mth);
	BW_ASSERT(dte <= 31 && dte >= 1, "Invalid date %d (Expected 1-31)", dte);
	BW_ASSERT(wd <= 7 && wd >= 1, "Invalid weekday %d (Expected 1-12)", wd);
	rtc_enable_init();

	uint32_t dr = 0x0UL;
	MODIFY_FIELD(dr, RTC_DR_YT_Msk, RTC_DR_YT_Pos, (yr / 10) % 10);
	MODIFY_FIELD(dr, RTC_DR_YU_Msk, RTC_DR_YU_Pos, yr % 10);
	MODIFY_FIELD(dr, RTC_DR_WDU_Msk, RTC_DR_WDU_Pos, wd % 10);
	MODIFY_FIELD(dr, RTC_DR_MT_Msk, RTC_DR_MT_Pos, (mth / 10) % 10);
	MODIFY_FIELD(dr, RTC_DR_MU_Msk, RTC_DR_MU_Pos, mth % 10);
	MODIFY_FIELD(dr, RTC_DR_DT_Msk, RTC_DR_DT_Pos, (dte / 10) % 10);
	MODIFY_FIELD(dr, RTC_DR_DU_Msk, RTC_DR_DU_Pos, dte % 10);
	RTC->DR = dr;

	rtc_disable_init();
}

uint32_t rtc_get_timestamp()
{
	uint8_t hr, min, sec, yr, mth, dte, wd;
	bool pm;

	rtc_get_time(&hr, &min, &sec, &pm);
	rtc_get_date(&yr, &mth, &dte, &wd);

	return ((uint32_t)(yr & 0x7F) << 25) | ((uint32_t)(mth & 0x0F) << 21) |
		   ((uint32_t)(dte & 0x1F) << 16) | ((uint32_t)(hr & 0x0F) << 12) |
		   ((uint32_t)(pm & 0x01) << 11) | ((uint32_t)(min & 0x3F) << 5) |
		   ((uint32_t)(sec & 0x3F) << 0);
}

uint32_t rtc_get_time_ms()
{
	rtc_sync_shadow_regs();

	uint32_t ssr, tr, dr;
	ssr = RTC->SSR;
	tr = RTC->TR;
	dr = RTC->DR;

	uint32_t predivs = RTC->PRER & RTC_PRER_PREDIV_S_Msk;

	uint8_t hr = READ_FIELD(tr, RTC_TR_HT_Msk, RTC_TR_HT_Pos) * 10 +
				 READ_FIELD(tr, RTC_TR_HU_Msk, RTC_TR_HU_Pos);
	uint8_t min = READ_FIELD(tr, RTC_TR_MNT_Msk, RTC_TR_MNT_Pos) * 10 +
				  READ_FIELD(tr, RTC_TR_MNU_Msk, RTC_TR_MNU_Pos);
	uint8_t sec = READ_FIELD(tr, RTC_TR_ST_Msk, RTC_TR_ST_Pos) * 10 +
				  READ_FIELD(tr, RTC_TR_SU_Msk, RTC_TR_SU_Pos);

	if (READ_BIT(RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12) {
		bool pm = (tr & RTC_TR_PM_Msk) != 0;
		uint8_t h12 = hr % 12;
		hr = pm ? (h12 + 12) : h12;
	}

	uint16_t yr = READ_FIELD(dr, RTC_DR_YT_Msk, RTC_DR_YT_Pos) * 10 +
				  READ_FIELD(dr, RTC_DR_YU_Msk, RTC_DR_YU_Pos);
	uint8_t mth = READ_FIELD(dr, RTC_DR_MT_Msk, RTC_DR_MT_Pos) * 10 +
				  READ_FIELD(dr, RTC_DR_MU_Msk, RTC_DR_MU_Pos);
	uint8_t dte = READ_FIELD(dr, RTC_DR_DT_Msk, RTC_DR_DT_Pos) * 10 +
				  READ_FIELD(dr, RTC_DR_DU_Msk, RTC_DR_DU_Pos);

	int32_t y = (int32_t)(2000 + yr) - (mth <= 2 ? 1 : 0);
	int32_t era = (y >= 0 ? y : y - 399) / 400;
	uint32_t yoe = (uint32_t)(y - era * 400);
	uint32_t doy =
		(153U * (uint32_t)(mth + (mth > 2 ? (uint32_t)0 - 3U : 9U)) + 2U) / 5U + dte - 1U;
	uint32_t doe = yoe * 365U + yoe / 4U - yoe / 100U + doy;
	int32_t days = era * 146097 + (int32_t)doe - 719468;

	uint32_t whole_seconds = (uint32_t)days * 86400UL + hr * 3600UL + min * 60UL + sec;

	uint32_t ssr_ticks = predivs - (ssr & RTC_SSR_SS_Msk);

	return whole_seconds * 1000UL + (ssr_ticks * 1000UL) / (predivs + 1UL);
}

static void rtc_wkup_isr(void *user_data)
{
	rtc_unlock();
	CLEAR_FIELD(RTC->ISR, RTC_ISR_WUTF_Msk);
	rtc_lock();

	uint32_t delay_s = (uint32_t)user_data;
	if (g_wkup_callback != NULL)
		g_wkup_callback(delay_s);
}

void rtc_enable_wut(uint16_t delay_s, wkup_callback_t callback, uint8_t irq_priority)
{
	rtc_unlock();

	// Disable wake-up timer
	CLEAR_FIELD(RTC->CR, RTC_CR_WUTE_Msk);
	while (!(RTC->ISR & RTC_ISR_WUTWF_Msk))
		;

	// Configure the value
	RTC->WUTR = delay_s - 1;
	MODIFY_FIELD(RTC->CR, RTC_CR_WUCKSEL_Msk, RTC_CR_WUCKSEL_Pos, 0x4);

	// Enable wake-up timer interrupt
	SET_FIELD(RTC->CR, RTC_CR_WUTIE_Msk);

	struct exti_conf exti_conf = { .im = WKUP_IM,
								   .edge = EXTI_EDGE_RISING,
								   .irq = RTC_WKUP_IRQn,
								   .irq_priority = irq_priority,
								   .callback = rtc_wkup_isr,
								   .user_data = (void *)(uint32_t)delay_s };
	exti_init(&exti_conf, &g_wkup_exti_h);

	// Enable wake-up timer
	SET_FIELD(RTC->CR, RTC_CR_WUTE_Msk);
	g_wkup_callback = callback;

	rtc_lock();
}

void rtc_disable_wut()
{
	rtc_unlock();
	exti_deinit(&g_wkup_exti_h);
	rtc_clear_isr(RTC_ISR_WUTF_Msk);
	CLEAR_FIELD(RTC->CR, RTC_CR_WUTIE_Msk);

	// disable wake-up timer
	CLEAR_FIELD(RTC->CR, RTC_CR_WUTE_Msk);
	while (!(RTC->ISR & RTC_ISR_WUTWF_Msk))
		;

	rtc_lock();
}

static void rtc_alrm_isr(void *user_data)
{
	(void)user_data;
	rtc_unlock();
	CLEAR_FIELD(RTC->ISR, RTC_ISR_ALRAF_Msk);
	rtc_lock();

	if (g_alrm_callback != NULL)
		g_alrm_callback();
}

void rtc_enable_alarm(uint8_t hr, uint8_t min, uint8_t sec, bool pm, uint32_t mask,
					  alrm_callback_t callback, uint8_t irq_priority)
{
	rtc_unlock();

	BW_ASSERT(min < 60, "Invalid minute %d (Expected 0-59)", min);
	BW_ASSERT(sec < 60, "Invalid second %d (Expected 0-59)", sec);

	CLEAR_FIELD(RTC->CR, RTC_CR_ALRAE_Msk);
	while (!(RTC->ISR & RTC_ISR_ALRAWF_Msk))
		;

	uint32_t alrmar = 0x0UL;
	SET_FIELD(alrmar, RTC_ALRMAR_MSK4_Msk | mask); // always mask the date
	if (READ_BIT(RTC->CR, RTC_CR_FMT_Pos) == RTC_HR_FMT_12) {
		BW_ASSERT(hr >= 1 && hr <= 12, "Invalid hour %d (Expected 1-12)", hr);
		MODIFY_BIT(alrmar, RTC_ALRMAR_PM_Pos, pm);
	} else {
		BW_ASSERT(hr < 24, "Invalid hour %d (Expected 0-23)", hr);
		CLEAR_FIELD(alrmar, RTC_ALRMAR_PM_Msk);
	}

	MODIFY_FIELD(alrmar, RTC_ALRMAR_HT_Msk, RTC_ALRMAR_HT_Pos, (hr / 10) % 10);
	MODIFY_FIELD(alrmar, RTC_ALRMAR_HU_Msk, RTC_ALRMAR_HU_Pos, hr % 10);
	MODIFY_FIELD(alrmar, RTC_ALRMAR_MNT_Msk, RTC_ALRMAR_MNT_Pos, (min / 10) % 10);
	MODIFY_FIELD(alrmar, RTC_ALRMAR_MNU_Msk, RTC_ALRMAR_MNU_Pos, min % 10);
	MODIFY_FIELD(alrmar, RTC_ALRMAR_ST_Msk, RTC_ALRMAR_ST_Pos, (sec / 10) % 10);
	MODIFY_FIELD(alrmar, RTC_ALRMAR_SU_Msk, RTC_ALRMAR_SU_Pos, sec % 10);
	RTC->ALRMAR = alrmar;

	SET_FIELD(RTC->CR, RTC_CR_ALRAIE_Msk);

	struct exti_conf exti_conf = { .im = ALRM_IM,
								   .edge = EXTI_EDGE_RISING,
								   .irq = RTC_Alarm_IRQn,
								   .irq_priority = irq_priority,
								   .callback = rtc_alrm_isr,
								   .user_data = NULL };
	exti_init(&exti_conf, &g_alrm_exti_h);

	SET_FIELD(RTC->CR, RTC_CR_ALRAE_Msk);
	g_alrm_callback = callback;

	rtc_lock();
}

void rtc_disable_alarm()
{
	rtc_unlock();
	exti_deinit(&g_alrm_exti_h);
	rtc_clear_isr(RTC_ISR_ALRAF_Msk);
	CLEAR_FIELD(RTC->CR, RTC_CR_ALRAIE_Msk);
	CLEAR_FIELD(RTC->CR, RTC_CR_ALRAE_Msk);
	rtc_lock();
}
