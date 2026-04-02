#include "hal/uart/uart.h"
#include "hal/gpio/gpio.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"

/*00: PCLK selected as USART1 clock
 *01: System clock (SYSCLK) selected as USART1 clock
 *10: HSI16 clock selected as USART1 clock
 *11: LSE clock selected as USART1 clock 
*/

void hal_uart_init(uart_conf_t *conf)
{
    gpio_conf_t gpio_conf = gpio_conf_af(conf->tx, conf->af, GPIO_TYPE_PUSH_PULL, GPIO_PULL_NONE);
    hal_gpio_init(&gpio_conf);

    // enable clock
    reg_set_mask(&RCC->APB2ENR, RCC_APB2ENR_USART1EN_Msk); 
    reg_set_field(&RCC->CCIPR, RCC_CCIPR_USART1SEL_Pos, 2, 0);

    // Set the M bit (8 bits)
    reg_clear_mask(&conf->uart->CR1, USART_CR1_M0_Msk);
    reg_clear_mask(&conf->uart->CR1, USART_CR1_M1_Msk);

    // Set the baud rate 
    reg_clear_mask(&conf->uart->CR1, USART_CR1_OVER8_Msk);
    reg_set_field(&conf->uart->BRR, 0, 16, USART_KER_CK / (USART_PRESC + 1) / conf->baud_rate);

    // Configure stop bits (1 bit)
    reg_set_field(&conf->uart->CR2, USART_CR2_STOP_Pos, 2, 0);

    // Enable USART 
    reg_set_mask(&conf->uart->CR1, USART_CR1_UE_Msk);

    // Send Idle frame
    reg_set_mask(&conf->uart->CR1, USART_CR1_TE_Msk);
}

void hal_uart_write(USART_TypeDef *uart, uint8_t* data, uint16_t len)
{
    while(len > 0)
    {
        // Wait till TXE = 1 to send next byte
        while(!(uart->ISR & USART_ISR_TXE));
        uart->TDR = *data++;
        len--;
    }
    
    // Wait till TC = 1
    while(!(uart->ISR & USART_ISR_TC));
}
