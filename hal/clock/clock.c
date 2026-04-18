#include "clock.h"
#include "reg.h"
#include "stm32wb55xx.h"

void hal_clock_init()
{
    // Set MSI to 16Mhz clk
    while ((RCC->CR & RCC_CR_MSION) && !(RCC->CR & RCC_CR_MSIRDY));
    reg_set_field(&RCC->CR, RCC_CR_MSIRANGE_Pos, 4, 0x8U);

    SystemCoreClockUpdate();
}
