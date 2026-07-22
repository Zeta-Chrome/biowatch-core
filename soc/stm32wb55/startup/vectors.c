#include "drivers/exti/exti.h"
#include "drivers/i2c/i2c.h"
#include "drivers/rtc/rtc.h"
#include "drivers/spi/spi.h"
#include "lib/logger.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"

void systick_tick();
void kernel_scheduler_tick();
void i2c_ev_isr(enum i2c_perip);
void i2c_er_isr(enum i2c_perip);
void spi_isr(enum spi_perip);
void dma_isr(DMA_TypeDef *, uint8_t);
void adc_isr();
void exti_isr(uint8_t);
void lptim_isr();
void ipcc_tx_isr();
void ipcc_rx_isr();
void hsem_isr();

void SysTick_Handler(void)
{
	systick_tick();
#ifdef DEBUG
	kernel_scheduler_tick();
#endif
}

void I2C1_EV_IRQHandler(void)
{
	i2c_ev_isr(I2C_PERIPH_1);
}

void I2C1_ER_IRQHandler(void)
{
	i2c_er_isr(I2C_PERIPH_1);
}

void I2C3_EV_IRQHandler(void)
{
	i2c_ev_isr(I2C_PERIPH_3);
}

void I2C3_ER_IRQHandler(void)
{
	i2c_er_isr(I2C_PERIPH_3);
}

void SPI1_IRQHandler(void)
{
	spi_isr(SPI_PERIPH_1);
}

void SPI2_IRQHandler(void)
{
	spi_isr(SPI_PERIPH_2);
}

// pins 0-4 get their own handler
void EXTI0_IRQHandler(void)
{
	exti_isr(0);
}

void EXTI1_IRQHandler(void)
{
	exti_isr(1);
}

void EXTI2_IRQHandler(void)
{
	exti_isr(2);
}

void EXTI3_IRQHandler(void)
{
	exti_isr(3);
}

void EXTI4_IRQHandler(void)
{
	exti_isr(4);
}

// pins 5-9
void EXTI9_5_IRQHandler(void)
{
	for (uint8_t i = 5; i <= 9; i++)
		if (READ_BIT(EXTI->PR1, i))
			exti_isr(i);
}

// pins 10-15
void EXTI15_10_IRQHandler(void)
{
	for (uint8_t i = 10; i <= 15; i++)
		if (READ_BIT(EXTI->PR1, i))
			exti_isr(i);
}

void DMA1_Channel1_IRQHandler(void)
{
	dma_isr(DMA1, 1);
}

void DMA1_Channel2_IRQHandler(void)
{
	dma_isr(DMA1, 2);
}

void DMA1_Channel3_IRQHandler(void)
{
	dma_isr(DMA1, 3);
}

void DMA1_Channel4_IRQHandler(void)
{
	dma_isr(DMA1, 4);
}

void DMA1_Channel5_IRQHandler(void)
{
	dma_isr(DMA1, 5);
}

void DMA1_Channel6_IRQHandler(void)
{
	dma_isr(DMA1, 6);
}

void DMA1_Channel7_IRQHandler(void)
{
	dma_isr(DMA1, 7);
}

void DMA2_Channel1_IRQHandler(void)
{
	dma_isr(DMA2, 1);
}

void DMA2_Channel2_IRQHandler(void)
{
	dma_isr(DMA2, 2);
}

void DMA2_Channel3_IRQHandler(void)
{
	dma_isr(DMA2, 3);
}

void DMA2_Channel4_IRQHandler(void)
{
	dma_isr(DMA2, 4);
}

void DMA2_Channel5_IRQHandler(void)
{
	dma_isr(DMA2, 5);
}

void DMA2_Channel6_IRQHandler(void)
{
	dma_isr(DMA2, 6);
}

void DMA2_Channel7_IRQHandler(void)
{
	dma_isr(DMA2, 7);
}

void RTC_WKUP_IRQHandler(void)
{
	exti_isr(WKUP_IM);
}

void RTC_Alarm_IRQHandler(void)
{
	exti_isr(ALRM_IM);
}

void ADC1_IRQHandler()
{
	adc_isr();
}

void LPTIM1_IRQHandler()
{
	lptim_isr();
}

void IPCC_C1_TX_IRQHandler()
{
	ipcc_tx_isr();
}

void IPCC_C1_RX_IRQHandler()
{
	ipcc_rx_isr();
}

void HSEM_IRQHandler()
{
	hsem_isr();
}

void HardFault_Handler(void)
{
	__asm volatile("bkpt #0");
	while (1)
		;
}

void MemManage_Handler(void)
{
	BW_LOG("Mem manage fault status: %x, MMFAR: %x\n", SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk,
		   SCB->MMFAR);
	__asm volatile("bkpt #0");
	while (1)
		;
}

void BusFault_Handler(void)
{
	BW_LOG("Bus fault status: %x, BFAR: %x\n", SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk, SCB->BFAR);
	__asm volatile("bkpt #0");
	while (1)
		;
}

void UsageFault_Handler(void)
{
	BW_LOG("Usage fault status: %x\n", SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk);
	__asm volatile("bkpt #0");
	while (1)
		;
}
