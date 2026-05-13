#include "clock.h"
#include "assert.h"
#include "clock_srcs.h"
#include "logger.h"
#include "reg.h"
#include "stm32wb55xx.h"

uint32_t SYSCLK_FREQ;
uint32_t HCLK1_FREQ;
uint32_t HCLK2_FREQ;
uint32_t HCLK4_FREQ;
uint32_t PCLK1_FREQ;
uint32_t PCLK2_FREQ;

static uint32_t g_msi_rmap[CLOCK_MSI_RANGE_48M + 1] = {100000,   200000,   400000,   800000,
                                                       1000000,  2000000,  4000000,  8000000,
                                                       16000000, 24000000, 32000000, 48000000};
static uint16_t g_hpre_map[CLOCK_HPRE_512 + 1] = {1, 3, 5, 6, 10, 32, 2, 4, 8, 16, 64, 128, 256, 512};

static bw_status_t hal_clock_configure_pll(clock_conf_t *conf)
{
    clock_pllr_conf_t *pllr = &conf->pllr;
    BW_ASSERT(pllr->mul >= 6 && pllr->mul <= 127, "Invalid PLLR mul %d (Expected 6-127)", pllr->mul);
    BW_ASSERT(pllr->div >= 1 && pllr->div <= 8, "Invalid PLLR div %d (Expected 1-8)", pllr->div);
    BW_ASSERT(pllr->rdiv >= 2 && pllr->rdiv <= 8, "Invalid PLLR rdiv %d (Expected 2-8)", pllr->rdiv);

    switch (pllr->src)
    {
    case CLOCK_SRC_MSI:
        hal_clock_enable_msi(conf->msi_range);
        reg_set_field(&RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Pos, 2, 0x1);
        SYSCLK_FREQ = g_msi_rmap[conf->msi_range];
        break;
    case CLOCK_SRC_HSI:
        hal_clock_enable_hsi();
        reg_set_field(&RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Pos, 2, 0x2);
        SYSCLK_FREQ = 16000000;
        break;
    case CLOCK_SRC_HSE:
        hal_clock_enable_hse();
        reg_set_field(&RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Pos, 2, 0x3);
        SYSCLK_FREQ = 32000000;
        break;
    default:
        BW_LOG("Invalid PLL clock source\n");
        return STATUS_ERR;
    }

    SYSCLK_FREQ = SYSCLK_FREQ * pllr->mul / pllr->div / pllr->rdiv;
    if (SYSCLK_FREQ > 64000000)
    {
        BW_LOG("SYSCLK frequency cannot be greater than 64MHz");
        return STATUS_ERR;
    }

    reg_set_field(&RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Pos, 7, pllr->mul);
    reg_set_field(&RCC->PLLCFGR, RCC_PLLCFGR_PLLM_Pos, 3, pllr->div - 1);
    reg_set_field(&RCC->PLLCFGR, RCC_PLLCFGR_PLLR_Pos, 3, pllr->rdiv - 1);
    reg_set_mask(&RCC->PLLCFGR, RCC_PLLCFGR_PLLREN_Msk);
    hal_clock_enable_pll();

    return STATUS_OK;
}

bw_status_t hal_clock_configure(clock_conf_t *conf)
{
    uint8_t clk_src;

    switch (conf->src)
    {
    case CLOCK_SRC_MSI:
        hal_clock_enable_msi(conf->msi_range);
        SYSCLK_FREQ = g_msi_rmap[conf->msi_range];
        clk_src = 0;
        break;
    case CLOCK_SRC_HSI:
        hal_clock_enable_hsi();
        SYSCLK_FREQ = 16000000;
        clk_src = 1;
        break;
    case CLOCK_SRC_HSE:
        hal_clock_enable_hse();
        SYSCLK_FREQ = 32000000;
        clk_src = 2;
        break;
    case CLOCK_SRC_PLL:
        if (hal_clock_configure_pll(conf) != STATUS_OK)
        {
            return STATUS_ERR;
        }
        clk_src = 3;
        break;
    }

    // hclk2 and hclk4 have sysclk frequencies
    HCLK2_FREQ = SYSCLK_FREQ;
    HCLK4_FREQ = SYSCLK_FREQ;

    // Set wait states in flash memory
    uint8_t latency = 0;
    if (HCLK4_FREQ <= 18000000)
    {
        latency = 0;
    }
    else if (HCLK4_FREQ <= 36000000)
    {
        latency = 1;
    } 
    else if (HCLK4_FREQ <= 54000000)
    {
        latency = 2;
    }
    else if (HCLK4_FREQ <= 64000000)
    {
        latency = 3;
    } 
    reg_set_field(&FLASH->ACR, FLASH_ACR_LATENCY_Pos, 2, latency);
    while((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != latency); // Wait for latency to set
    
    // Set clock source
    reg_set_field(&RCC->CFGR, RCC_CFGR_SW_Pos, 2, clk_src);
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != (clk_src << RCC_CFGR_SWS_Pos));

    if (conf->src != CLOCK_SRC_MSI && conf->pllr.src != CLOCK_SRC_MSI)
    {
        hal_clock_disable_msi();
    }
    
    // Configure hclk1 prescalar
    HCLK1_FREQ = SYSCLK_FREQ / g_hpre_map[conf->hpre]; 
    reg_set_field(&RCC->CFGR, RCC_CFGR_HPRE_Pos, 4, conf->hpre);
    while (!(RCC->CFGR & RCC_CFGR_HPREF_Msk));

    // Configure pclk1 prescalar
    PCLK1_FREQ = SYSCLK_FREQ / (1 << conf->ppre1);
    if (PCLK1_FREQ > 32000000)
    {
        BW_LOG("PCLK1 cannot be greater than 32MHz\n");
        return STATUS_ERR;
    }
    reg_set_field(&RCC->CFGR, RCC_CFGR_PPRE1_Pos, 3, conf->ppre1);
    while (!(RCC->CFGR & RCC_CFGR_PPRE1F_Msk));

    // Configure pclk2 prescalar
    PCLK2_FREQ = SYSCLK_FREQ / (1 << conf->ppre2); 
    reg_set_field(&RCC->CFGR, RCC_CFGR_PPRE2_Pos, 3, conf->ppre2);
    while (!(RCC->CFGR & RCC_CFGR_PPRE2F_Msk));

    return STATUS_OK;
}
