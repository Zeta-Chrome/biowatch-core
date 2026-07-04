#include "gpio/gpio.h"
#include "stm32wb55xx.h"
#include "uart.h"
#include "utils.h"

/*00: PCLK selected as USART1 clock
 *01: System clock (SYSCLK) selected as USART1 clock
 *10: HSI16 clock selected as USART1 clock
 *11: LSE clock selected as USART1 clock
 */

void hal_uart_init(uart_conf_t *conf)
{
    conf->uart->CR1 = 0;
    conf->uart->CR2 = 0;

    gpio_conf_t gpio_conf = gpio_conf_af(conf->tx, conf->af, GPIO_TYPE_PUSH_PULL, GPIO_PULL_NONE);
    hal_gpio_init(&gpio_conf);

    // enable clock
    MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_USART1SEL_Msk, RCC_CCIPR_USART1SEL_Pos, 0);
    SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_USART1EN_Msk);

    // Set the M bit (8 bits)
    CLEAR_FIELD(conf->uart->CR1, USART_CR1_M0_Msk | USART_CR1_M1_Msk);

    // Set the baud rate
    CLEAR_FIELD(conf->uart->CR1, USART_CR1_OVER8_Msk);
    MODIFY_FIELD(conf->uart->BRR, 0, 16, USART_KER_CK / (USART_PRESC + 1) / conf->baud_rate);

    // Configure stop bits (1 bit)
    MODIFY_FIELD(conf->uart->CR2, USART_CR2_STOP_Msk, USART_CR2_STOP_Pos, 0);

    // Enable USART
    SET_FIELD(conf->uart->CR1, USART_CR1_UE_Msk);

    // Send Idle frame
    SET_FIELD(conf->uart->CR1, USART_CR1_TE_Msk);
}

void hal_uart_write(USART_TypeDef *uart, uint8_t *data, uint16_t len)
{
    while (len > 0)
    {
        // Wait till TXE = 1 to send next byte
        while (!(uart->ISR & USART_ISR_TXE));
        uart->TDR = *data++;
        len--;
    }

    // Wait till TC = 1
    while (!(uart->ISR & USART_ISR_TC));
}

void hal_uart_deinit(USART_TypeDef *uart)
{
    // Disable USART
    CLEAR_FIELD(uart->CR1, USART_CR1_UE_Msk);

    // Disable USART clock
    CLEAR_FIELD(RCC->APB2ENR, RCC_APB2ENR_USART1EN_Msk);
}
