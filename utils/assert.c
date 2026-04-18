
#ifdef DEBUG

#include "assert.h"
#include "stm32wb55xx.h"

void bw_assert_handler()
{
    __disable_irq();
    while (1);
}

#endif
