#include "stm32wb55xx.h"
#include "system_stm32wbxx.h"

void SystemInit(void)
{

#if defined(USER_VECT_TAB_ADDRESS)
    /* Configure the Vector Table location add offset address ------------------*/
    SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif /* USER_VECT_TAB_ADDRESS */

    /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << (10UL * 2UL)) | (3UL << (11UL * 2UL))); /* set CP10 and CP11 Full Access */
#endif                                                             /* FPU */

    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set MSION bit */
    RCC->CR |= RCC_CR_MSION;

    /* Reset CFGR register */
    RCC->CFGR = 0x00070000U;

    /* Reset PLLSAI1ON, PLLON, HSECSSON, HSEON, HSION, and MSIPLLON bits */
    RCC->CR &= (uint32_t)0xFAF6FEFBU;

    /*!< Reset LSI1 and LSI2 bits */
    RCC->CSR &= (uint32_t)0xFFFFFFFAU;

    /*!< Reset HSI48ON  bit */
    RCC->CRRCR &= (uint32_t)0xFFFFFFFEU;

    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x22041000U;

#if defined(STM32WB55xx) || defined(STM32WB5Mxx)
    /* Reset PLLSAI1CFGR register */
    RCC->PLLSAI1CFGR = 0x22041000U;
#endif /* STM32WB55xx || STM32WB5Mxx */

    /* Reset HSEBYP bit */
    RCC->CR &= 0xFFFBFFFFU;

    /* Disable all interrupts */
    RCC->CIER = 0x00000000;
}
