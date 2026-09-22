#include "clock.h"
#include "clock_srcs.h"
#include "drivers/flash/flash.h"
#include "drivers/hsem/hsem.h"
#include "lib/assert.h"
#include "lib/logger.h"
#include "lib/status.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"

uint32_t SYSCLK_FREQ;
uint32_t HCLK1_FREQ;
uint32_t HCLK2_FREQ;
uint32_t HCLK4_FREQ;
uint32_t PCLK1_FREQ;
uint32_t PCLK2_FREQ;

static uint32_t g_msi_freq[CLOCK_MSI_RANGE_48M + 1] = { 100000,	  200000,	400000,	  800000,
														1000000,  2000000,	4000000,  8000000,
														16000000, 24000000, 32000000, 48000000 };
static uint8_t g_hpre_presc[CLOCK_HPRE_512 + 1] = { 0, 1, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static uint16_t g_hpre_div[CLOCK_HPRE_512 + 1] = { 1, 3, 5,	 6,	 10,  32,  2,
												   4, 8, 16, 64, 128, 256, 512 };
static uint8_t g_ppre_presc[CLOCK_PPRE_16 + 1] = { 0, 4, 5, 6, 7 };

static enum bw_status clock_configure_pll(struct clock_conf *conf)
{
	struct clock_pllr_conf *pllr = &conf->pllr;
	BW_ASSERT(pllr->mul >= 6 && pllr->mul <= 127, "Invalid PLLR mul %d (Expected 6-127)",
			  pllr->mul);
	BW_ASSERT(pllr->div >= 1 && pllr->div <= 8, "Invalid PLLR div %d (Expected 1-8)", pllr->div);
	BW_ASSERT(pllr->rdiv >= 2 && pllr->rdiv <= 8, "Invalid PLLR rdiv %d (Expected 2-8)",
			  pllr->rdiv);

	switch (pllr->src) {
	case CLOCK_SRC_MSI:
		clock_enable_msi(conf->msi_range);
		MODIFY_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_Pos, 0x1);
		SYSCLK_FREQ = g_msi_freq[conf->msi_range];
		break;
	case CLOCK_SRC_HSI:
		clock_enable_hsi();
		MODIFY_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_Pos, 0x2);
		SYSCLK_FREQ = 16000000;
		break;
	case CLOCK_SRC_HSE:
		clock_enable_hse();
		MODIFY_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_Pos, 0x3);
		SYSCLK_FREQ = 32000000;
		break;
	default:
		BW_LOG("Invalid PLL clock source\n");
		return STATUS_ERR;
	}

	SYSCLK_FREQ = SYSCLK_FREQ * pllr->mul / pllr->div / pllr->rdiv;
	if (SYSCLK_FREQ > 64000000) {
		BW_LOG("SYSCLK frequency cannot be greater than 64MHz");
		return STATUS_ERR;
	}

	MODIFY_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_Pos, pllr->mul);
	MODIFY_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_Msk, RCC_PLLCFGR_PLLM_Pos, pllr->div - 1);
	MODIFY_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLR_Msk, RCC_PLLCFGR_PLLR_Pos, pllr->rdiv - 1);
	SET_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN_Msk);
	clock_enable_pll();

	return STATUS_OK;
}

static enum bw_status enable_clocks(struct clock_conf *conf)
{
	hsem_fast_lock_spin(HSEM_ID_RCC);
	switch (conf->src) {
	case CLOCK_SRC_MSI:
		clock_enable_msi(conf->msi_range);
		SYSCLK_FREQ = g_msi_freq[conf->msi_range];
		break;
	case CLOCK_SRC_HSI:
		clock_enable_hsi();
		SYSCLK_FREQ = 16000000;
		break;
	case CLOCK_SRC_HSE:
		clock_enable_hse();
		SYSCLK_FREQ = 32000000;
		break;
	case CLOCK_SRC_PLL:
		if (clock_configure_pll(conf) != STATUS_OK) {
			hsem_fast_unlock(HSEM_ID_RCC);
			return STATUS_ERR;
		}
		break;
	default:
		hsem_fast_unlock(HSEM_ID_RCC);
		return STATUS_ERR;
	}
	hsem_fast_unlock(HSEM_ID_RCC);

	return STATUS_OK;
}

static enum bw_status configure_bus_clocks(struct clock_conf *conf)
{
	hsem_fast_lock_spin(HSEM_ID_RCC);

	// Configure hclk1 prescalar
	HCLK1_FREQ = SYSCLK_FREQ / g_hpre_div[conf->hpre1];
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_HPRE_Msk, RCC_CFGR_HPRE_Pos, g_hpre_presc[conf->hpre1]);
	while (!(RCC->CFGR & RCC_CFGR_HPREF_Msk))
		;

	// Configure hclk2 prescaler
	HCLK2_FREQ = SYSCLK_FREQ / g_hpre_div[conf->hpre2];
	if (HCLK2_FREQ > 32000000) {
		BW_LOG("HCLK2 cannot be greater than 32MHz\n");
		hsem_fast_unlock(HSEM_ID_RCC);
		return STATUS_ERR;
	}

	MODIFY_FIELD(RCC->EXTCFGR, RCC_EXTCFGR_C2HPRE_Msk, RCC_EXTCFGR_C2HPRE_Pos,
				 g_hpre_presc[conf->hpre2]);
	while (!(RCC->EXTCFGR & RCC_EXTCFGR_C2HPREF_Msk))
		;

	// configure hclk4 prescaler
	HCLK4_FREQ = SYSCLK_FREQ / g_hpre_div[conf->hpre4];
	if (HCLK4_FREQ > 32000000) {
		BW_LOG("HCLK4 cannot be greater than 32MHz\n");
		hsem_fast_unlock(HSEM_ID_RCC);
		return STATUS_ERR;
	}
	MODIFY_FIELD(RCC->EXTCFGR, RCC_EXTCFGR_SHDHPRE_Msk, RCC_EXTCFGR_SHDHPRE_Pos,
				 g_hpre_presc[conf->hpre4]);
	while (!(RCC->EXTCFGR & RCC_EXTCFGR_SHDHPREF_Msk))
		;

	// Configure pclk1 prescalar
	PCLK1_FREQ = SYSCLK_FREQ >> conf->ppre1;
	if (PCLK1_FREQ > 32000000) {
		BW_LOG("PCLK1 cannot be greater than 32MHz\n");
		hsem_fast_unlock(HSEM_ID_RCC);
		return STATUS_ERR;
	}
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_Pos, g_ppre_presc[conf->ppre1]);
	while (!(RCC->CFGR & RCC_CFGR_PPRE1F_Msk))
		;

	// Configure pclk2 prescalar
	PCLK2_FREQ = SYSCLK_FREQ >> conf->ppre2;
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_Pos, g_ppre_presc[conf->ppre2]);
	while (!(RCC->CFGR & RCC_CFGR_PPRE2F_Msk))
		;

	hsem_fast_unlock(HSEM_ID_RCC);

	return STATUS_OK;
}

static void select_clock(struct clock_conf *conf, uint32_t old_hclk4)
{
	hsem_fast_lock_spin(HSEM_ID_RCC);
	// If frequency is going UP, raise wait states before switching
	if (HCLK4_FREQ > old_hclk4)
		flash_configure_latency(HCLK4_FREQ);

	// Set clock source
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, conf->src);
	while (READ_FIELD(RCC->CFGR, RCC_CFGR_SWS_Msk, RCC_CFGR_SWS_Pos) != conf->src)
		;

	// If frequency went DOWN, lower wait states only after the switch
	if (HCLK4_FREQ <= old_hclk4)
		flash_configure_latency(HCLK4_FREQ);
	hsem_fast_unlock(HSEM_ID_RCC);
}

enum bw_status clock_configure(struct clock_conf *conf)
{
	uint32_t old_sysclk = SYSCLK_FREQ;
	uint32_t old_hclk4 = HCLK4_FREQ;

	enum bw_status status = enable_clocks(conf);
	if (status != STATUS_OK)
		return status;

	if (SYSCLK_FREQ > old_sysclk) {
		status = configure_bus_clocks(conf);
		if (status != STATUS_OK)
			return status;
		select_clock(conf, old_hclk4);
	} else {
		select_clock(conf, old_hclk4);
		status = configure_bus_clocks(conf);
	}

	return status;
}

enum bw_status clock_reconfigure(struct clock_conf *conf)
{
	if (clock_configure(conf) != STATUS_OK)
		return STATUS_ERR;

	hsem_fast_lock_spin(HSEM_ID_RCC);
	// Disable the rest of the internal clocks
	if (conf->src != CLOCK_SRC_MSI && conf->pllr.src != CLOCK_SRC_MSI)
		clock_disable_msi();

	if (conf->src != CLOCK_SRC_HSI && conf->pllr.src != CLOCK_SRC_HSI)
		clock_disable_hsi();

	// Used as source for ble
	// if (conf->src != CLOCK_SRC_HSE && conf->pllr.src != CLOCK_SRC_HSE)
	// 	clock_disable_hse();

	if (conf->src != CLOCK_SRC_PLL) {
		CLEAR_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN_Msk);
		clock_disable_pll();
	}
	hsem_fast_unlock(HSEM_ID_RCC);

	return STATUS_OK;
}
