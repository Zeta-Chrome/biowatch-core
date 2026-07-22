#include "drivers/clock/clock.h"
#include "drivers/gpio/gpio.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"
#include "uart.h"

/*00: PCLK selected as USART1 clock
 *01: System clock (SYSCLK) selected as USART1 clock
 *10: HSI16 clock selected as USART1 clock
 *11: LSE clock selected as USART1 clock
 */

void uart_init(struct uart_conf *conf)
{
	conf->uart->CR1 = 0;
	conf->uart->CR2 = 0;

	struct gpio_conf gpio_conf =
		gpio_conf_af(conf->tx, conf->af, GPIO_TYPE_PUSH_PULL, GPIO_PULL_NONE);
	gpio_init(&gpio_conf);
	MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_USART1SEL_Msk, RCC_CCIPR_USART1SEL_Pos, 0);
	SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_USART1EN_Msk);
	CLEAR_FIELD(conf->uart->CR1, USART_CR1_M0_Msk | USART_CR1_M1_Msk);
	CLEAR_FIELD(conf->uart->CR1, USART_CR1_OVER8_Msk);
	WRITE_FIELD(conf->uart->BRR, PCLK1_FREQ / conf->baud_rate);
	MODIFY_FIELD(conf->uart->CR2, USART_CR2_STOP_Msk, USART_CR2_STOP_Pos, 0);
	SET_FIELD(conf->uart->CR1, USART_CR1_UE_Msk);
	SET_FIELD(conf->uart->CR1, USART_CR1_TE_Msk);
}

void uart_write(USART_TypeDef *uart, uint8_t *data, uint16_t len)
{
	while (len > 0) {
		while (!(uart->ISR & USART_ISR_TXE))
			;
		uart->TDR = *data++;
		len--;
	}

	while (!(uart->ISR & USART_ISR_TC))
		;
}

void uart_deinit(USART_TypeDef *uart)
{
	CLEAR_FIELD(uart->CR1, USART_CR1_UE_Msk);
	CLEAR_FIELD(RCC->APB2ENR, RCC_APB2ENR_USART1EN_Msk);
}
