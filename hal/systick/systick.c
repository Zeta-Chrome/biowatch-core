#include "hal/systick/systick.h"
#include "device/stm32wb55xx.h"
#include "cmsis/core_cm4.h"

#define SYSTICK_CLKSOURCE 1UL
#define CYCLES_PER_MS (SystemCoreClock / 1000)
#define CYCLES_PER_US (SystemCoreClock / 1000000)
#define SYSTICK_LOAD ((CYCLES_PER_MS - 1U) & 0x00FFFFFFU) 

static volatile uint32_t g_tick_ms = 0;

void hal_systick_init()
{
    g_tick_ms = 0;
    SysTick->CTRL = 0;
    SysTick->LOAD = SYSTICK_LOAD;
    SysTick->VAL = 0;
    SysTick->CTRL = (SYSTICK_CLKSOURCE << SysTick_CTRL_CLKSOURCE_Pos) | SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

void hal_systick_tick()
{
    g_tick_ms++;
}

uint32_t hal_systick_millis()
{
    return g_tick_ms;
}

uint32_t hal_systick_micros()
{
    uint32_t ms, val;
    do 
    {
        ms = g_tick_ms;
        val = (SysTick->VAL & 0x00FFFFFFU);
    }
    while(ms != g_tick_ms); // race condition
    
    return ms * 1000 + (SYSTICK_LOAD - val) / CYCLES_PER_US;
}

void hal_systick_delay_ms(uint32_t ms) 
{
    if (ms == 0)
    {
        return;
    }
    uint32_t start = hal_systick_millis();
    while(hal_systick_millis() - start < ms);
}

void hal_systick_delay_us(uint32_t us) 
{
    if (us == 0)
    {
        return;
    }
    uint32_t start = hal_systick_micros();
    while(hal_systick_micros() - start < us);
}
