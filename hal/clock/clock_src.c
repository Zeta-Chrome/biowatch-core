#include "clock_srcs.h"
#include "reg.h"
#include "stm32wb55xx.h"

void hal_clock_enable_msi(clock_msi_range_t range)
{
    if ((RCC->CR & RCC_CR_MSION_Msk) && (RCC->CR & RCC_CR_MSIRDY_Msk))
    {
        return ; 
    }

    reg_set_mask(&RCC->CR, RCC_CR_MSION_Msk);
    reg_set_field(&RCC->CR, RCC_CR_MSIRANGE_Pos, 4, range);
    while (!(RCC->CR & RCC_CR_MSIRDY_Msk));
}

void hal_clock_disable_msi()
{
    reg_clear_mask(&RCC->CR, RCC_CR_MSION_Msk);
    while (RCC->CR & RCC_CR_MSIRDY_Msk);
}

void hal_clock_enable_hsi()
{
    if ((RCC->CR & RCC_CR_HSION_Msk) && (RCC->CR & RCC_CR_HSIRDY_Msk))
    {
        return ; 
    }

    reg_set_mask(&RCC->CR, RCC_CR_HSION_Msk);
    while (!(RCC->CR & RCC_CR_HSIRDY_Msk));
}

void hal_clock_disable_hsi()
{
    reg_clear_mask(&RCC->CR, RCC_CR_HSION_Msk);
    while (RCC->CR & RCC_CR_HSIRDY_Msk);
}

void hal_clock_enable_hse()
{
    if ((RCC->CR & RCC_CR_HSEON_Msk) && (RCC->CR & RCC_CR_HSERDY_Msk))
    {
        return ; 
    }

    reg_set_mask(&RCC->CR, RCC_CR_HSEON_Msk);
    while (!(RCC->CR & RCC_CR_HSERDY_Msk));
}

void hal_clock_disable_hse()
{
    reg_clear_mask(&RCC->CR, RCC_CR_HSEON_Msk);
    while (RCC->CR & RCC_CR_HSERDY_Msk);
}

void hal_clock_enable_pll()
{
    if ((RCC->CR & RCC_CR_PLLON_Msk) && (RCC->CR & RCC_CR_PLLRDY_Msk))
    {
        return ; 
    }

    reg_set_mask(&RCC->CR, RCC_CR_PLLON_Msk);
    while (!(RCC->CR & RCC_CR_PLLRDY_Msk));
}

void hal_clock_disable_pll()
{
    reg_clear_mask(&RCC->CR, RCC_CR_PLLON_Msk);
    while (RCC->CR & RCC_CR_PLLRDY_Msk);
}

void hal_clock_enable_lse()
{
    if ((RCC->BDCR & RCC_BDCR_LSEON_Msk) && (RCC->BDCR & RCC_BDCR_LSERDY_Msk))
    {
        return ; 
    }

    reg_set_mask(&RCC->BDCR, RCC_BDCR_LSEON_Msk);
    while (!(RCC->BDCR & RCC_BDCR_LSERDY_Pos));
}

void hal_clock_disable_lse()
{
    reg_clear_mask(&RCC->BDCR, RCC_BDCR_LSEON_Msk);
    while (RCC->BDCR & RCC_BDCR_LSERDY_Pos);
}
