#include "hal/i2c/i2c.h"
#include "hal/spi/spi.h"
#include "hal/exti/exti.h"
#include "hal/rtc/rtc.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"

void hal_systick_tick();
void rtos_scheduler_tick();
void hal_i2c_ev_isr(i2c_perip_t);
void hal_i2c_er_isr(i2c_perip_t);
void hal_spi_isr(spi_perip_t);
void hal_dma_isr(DMA_TypeDef *, uint8_t);
void hal_adc_isr();
void hal_lptim_isr();
  
void SysTick_Handler(void)
{
    hal_systick_tick();
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

void DMA1_Channel1_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 1);
}

void DMA1_Channel2_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 2);
}

void DMA1_Channel3_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 3);
}

void DMA1_Channel4_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 4);
}

void DMA1_Channel5_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 5);
}

void DMA1_Channel6_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 6);
}

void DMA1_Channel7_IRQHandler(void) 
{
    hal_dma_isr(DMA1, 7);
}

void DMA2_Channel1_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 1);
}

void DMA2_Channel2_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 2);
}

void DMA2_Channel3_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 3);
}

void DMA2_Channel4_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 4);
}

void DMA2_Channel5_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 5);
}

void DMA2_Channel6_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 6);
}

void DMA2_Channel7_IRQHandler(void) 
{
    hal_dma_isr(DMA2, 7);
}

void RTC_WKUP_IRQHandler(void)
{
    hal_exti_isr(WKUP_IM);
}

void RTC_Alarm_IRQHandler(void)
{
    hal_exti_isr(ALRM_IM);
}

void ADC1_IRQHandler()
{
    hal_adc_isr();
}

void LPTIM1_IRQHandler()
{
    hal_lptim_isr();
}

void HardFault_Handler(void)
{
    __asm volatile("bkpt #0");
    while(1);
}

void MemManage_Handler(void)
{
    __asm volatile("bkpt #0");
    while(1);
}

void BusFault_Handler(void)
{
    __asm volatile("bkpt #0");
    while(1);
}

void UsageFault_Handler(void)
{
    __asm volatile("bkpt #0");
    while(1);
}
