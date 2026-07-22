#include "clock_srcs.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"

void clock_enable_msi(enum clock_msi_range range)
{
	if ((RCC->CR & RCC_CR_MSION_Msk) && (RCC->CR & RCC_CR_MSIRDY_Msk))
		return;

	SET_FIELD(RCC->CR, RCC_CR_MSION_Msk);
	MODIFY_FIELD(RCC->CR, RCC_CR_MSIRANGE_Msk, RCC_CR_MSIRANGE_Pos, range);
	while (!(RCC->CR & RCC_CR_MSIRDY_Msk))
		;
}

void clock_disable_msi()
{
	CLEAR_FIELD(RCC->CR, RCC_CR_MSION_Msk);
	while (RCC->CR & RCC_CR_MSIRDY_Msk)
		;
}

void clock_enable_hsi()
{
	if ((RCC->CR & RCC_CR_HSION_Msk) && (RCC->CR & RCC_CR_HSIRDY_Msk))
		return;

	SET_FIELD(RCC->CR, RCC_CR_HSION_Msk);
	while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
		;
}

void clock_disable_hsi()
{
	CLEAR_FIELD(RCC->CR, RCC_CR_HSION_Msk);
	while (RCC->CR & RCC_CR_HSIRDY_Msk)
		;
}

void clock_enable_hse()
{
	if ((RCC->CR & RCC_CR_HSEON_Msk) && (RCC->CR & RCC_CR_HSERDY_Msk))
		return;

	SET_FIELD(RCC->CR, RCC_CR_HSEON_Msk);
	while (!(RCC->CR & RCC_CR_HSERDY_Msk))
		;
}

void clock_disable_hse()
{
	CLEAR_FIELD(RCC->CR, RCC_CR_HSEON_Msk);
	while (RCC->CR & RCC_CR_HSERDY_Msk)
		;
}

void clock_enable_pll()
{
	if ((RCC->CR & RCC_CR_PLLON_Msk) && (RCC->CR & RCC_CR_PLLRDY_Msk))
		return;

	SET_FIELD(RCC->CR, RCC_CR_PLLON_Msk);
	while (!(RCC->CR & RCC_CR_PLLRDY_Msk))
		;
}

void clock_disable_pll()
{
	CLEAR_FIELD(RCC->CR, RCC_CR_PLLON_Msk);
	while (RCC->CR & RCC_CR_PLLRDY_Msk)
		;
}

void clock_enable_lse()
{
	if ((RCC->BDCR & RCC_BDCR_LSEON_Msk) && (RCC->BDCR & RCC_BDCR_LSERDY_Msk))
		return;

	SET_FIELD(RCC->BDCR, RCC_BDCR_LSEON_Msk);
	while (!(RCC->BDCR & RCC_BDCR_LSERDY_Pos))
		;
}

void clock_disable_lse()
{
	CLEAR_FIELD(RCC->BDCR, RCC_BDCR_LSEON_Msk);
	while (RCC->BDCR & RCC_BDCR_LSERDY_Pos)
		;
}
