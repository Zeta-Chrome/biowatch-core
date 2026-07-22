#include "clock.h"
#include "clock_srcs.h"
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

static uint32_t g_msi_rmap[CLOCK_MSI_RANGE_48M + 1] = { 100000,	  200000,	400000,	  800000,
														1000000,  2000000,	4000000,  8000000,
														16000000, 24000000, 32000000, 48000000 };
static uint16_t g_hpre_map[CLOCK_HPRE_512 + 1] = { 1, 3, 5,	 6,	 10,  32,  2,
												   4, 8, 16, 64, 128, 256, 512 };

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
		SYSCLK_FREQ = g_msi_rmap[conf->msi_range];
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

static void configure_flash_latency(uint32_t freq)
{
	uint8_t latency = 0;
	if (freq <= 18000000)
		latency = 0;
	else if (freq <= 36000000)
		latency = 1;
	else if (freq <= 54000000)
		latency = 2;
	else if (freq <= 64000000)
		latency = 3;

	MODIFY_FIELD(FLASH->ACR, FLASH_ACR_LATENCY_Msk, FLASH_ACR_LATENCY_Pos, latency);
	while (READ_FIELD(FLASH->ACR, FLASH_ACR_LATENCY_Msk, FLASH_ACR_LATENCY_Pos) != latency)
		;
}

enum bw_status clock_configure(struct clock_conf *conf)
{
	uint8_t clk_src;

	switch (conf->src) {
	case CLOCK_SRC_MSI:
		clock_enable_msi(conf->msi_range);
		SYSCLK_FREQ = g_msi_rmap[conf->msi_range];
		clk_src = 0;
		break;
	case CLOCK_SRC_HSI:
		clock_enable_hsi();
		SYSCLK_FREQ = 16000000;
		clk_src = 1;
		break;
	case CLOCK_SRC_HSE:
		clock_enable_hse();
		SYSCLK_FREQ = 32000000;
		clk_src = 2;
		break;
	case CLOCK_SRC_PLL:
		if (clock_configure_pll(conf) != STATUS_OK)
			return STATUS_ERR;

		clk_src = 3;
		break;
	default:
		return STATUS_ERR;
	}

	// hclk2 and hclk4 have sysclk frequencies
	HCLK2_FREQ = SYSCLK_FREQ;
	HCLK4_FREQ = SYSCLK_FREQ;

	// Set wait states in flash memory
	configure_flash_latency(HCLK4_FREQ);

	// Set clock source
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_SW_Msk, RCC_CFGR_SW_Pos, clk_src);
	while (READ_FIELD(RCC->CFGR, RCC_CFGR_SWS_Msk, RCC_CFGR_SWS_Pos) != clk_src)
		;

	// Configure hclk1 prescalar
	HCLK1_FREQ = SYSCLK_FREQ / g_hpre_map[conf->hpre];
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_HPRE_Msk, RCC_CFGR_HPRE_Pos, conf->hpre);
	while (!(RCC->CFGR & RCC_CFGR_HPREF_Msk))
		;

	// Configure pclk1 prescalar
	PCLK1_FREQ = SYSCLK_FREQ / (1 << conf->ppre1);
	if (PCLK1_FREQ > 32000000) {
		BW_LOG("PCLK1 cannot be greater than 32MHz\n");
		return STATUS_ERR;
	}
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_Pos, conf->ppre1);
	while (!(RCC->CFGR & RCC_CFGR_PPRE1F_Msk))
		;

	// Configure pclk2 prescalar
	PCLK2_FREQ = SYSCLK_FREQ / (1 << conf->ppre2);
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_Pos, conf->ppre2);
	while (!(RCC->CFGR & RCC_CFGR_PPRE2F_Msk))
		;

	return STATUS_OK;
}

enum bw_status clock_reconfigure(struct clock_conf *conf)
{
	if (clock_configure(conf) != STATUS_OK)
		return STATUS_ERR;

	// Disable the rest of the clocks
	if (conf->src != CLOCK_SRC_MSI && conf->pllr.src != CLOCK_SRC_MSI)
		clock_disable_msi();

	if (conf->src != CLOCK_SRC_HSI && conf->pllr.src != CLOCK_SRC_HSI)
		clock_disable_hsi();

	if (conf->src != CLOCK_SRC_HSE && conf->pllr.src != CLOCK_SRC_HSE)
		clock_disable_hse();

	if (conf->src != CLOCK_SRC_PLL) {
		CLEAR_FIELD(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN_Msk);
		clock_disable_pll();
	}

	return STATUS_OK;
}
