#include "hal/i2c/i2c.h"
#include "hal/spi/spi.h"
#include "hal/exti/exti.h"
#include "hal/rtc/rtc.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"

void hal_systick_tick();
void scheduler_tick(); // Present in the firmware

void SysTick_Handler(void)
{
    hal_systick_tick();
    scheduler_tick();
}

void I2C1_EV_IRQHandler(void)
{
    hal_i2c_ev_isr(I2C_PERIPH_1);
}

void I2C1_ER_IRQHandler(void)
{
    hal_i2c_er_isr(I2C_PERIPH_1);
}

void I2C3_EV_IRQHandler(void)
{
    hal_i2c_ev_isr(I2C_PERIPH_3);
}

void I2C3_ER_IRQHandler(void)
{
    hal_i2c_er_isr(I2C_PERIPH_3);
}

void SPI1_IRQHandler(void)
{
    hal_spi_isr(SPI_PERIPH_1);
}

void SPI2_IRQHandler(void)
{
    hal_spi_isr(SPI_PERIPH_2);
}

// pins 0-4 get their own handler
void EXTI0_IRQHandler(void)
{
    hal_exti_isr(0);
}

void EXTI1_IRQHandler(void)
{
    hal_exti_isr(1);
}

void EXTI2_IRQHandler(void)
{
    hal_exti_isr(2);
}

void EXTI3_IRQHandler(void)
{
    hal_exti_isr(3);
}

void EXTI4_IRQHandler(void)
{
    hal_exti_isr(4);
}

// pins 5-9
void EXTI9_5_IRQHandler(void)
{
    for (uint8_t i = 5; i <= 9; i++)
        if (reg_get_bit(&EXTI->PR1, i))
            hal_exti_isr(i);
}

// pins 10-15
void EXTI15_10_IRQHandler(void)
{
    for (uint8_t i = 10; i <= 15; i++)
        if (reg_get_bit(&EXTI->PR1, i))
            hal_exti_isr(i);
}

void RTC_WKUP_IRQHandler(void)
{
    hal_rtc_wkup_isr();
}

void RTC_Alarm_IRQHandler(void)
{
    hal_rtc_alrm_isr();
}
