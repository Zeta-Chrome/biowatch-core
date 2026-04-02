#include "hal/i2c/i2c.h"
#include "hal/gpio/gpio.h"
#include "hal/gpio/gpio_types.h"
#include "hal/reg.h"
#include "utils/status.h"
#include "utils/assert.h"
#include "stm32wb55xx.h"
#include <stdint.h>

static i2c_handle_t *g_i2c_handles[MAX_I2C_PERIPHERALS];

static void i2c_clock_init(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)
    {
        reg_set_field(&RCC->CCIPR, RCC_CCIPR_I2C1SEL_Pos, 2, 0x1);  // System Clock
        reg_set_mask(&RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN_Msk);
    }
    else if (i2c == I2C3)
    {
        reg_set_field(&RCC->CCIPR, RCC_CCIPR_I2C3SEL_Pos, 2, 0x1);  // System Clock
        reg_set_mask(&RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN_Msk);
    }
}

static void i2c_gpio_init(i2c_conf_t *conf)
{
    gpio_conf_t gpio_conf = gpio_conf_af(conf->sda, conf->af, GPIO_TYPE_OPEN_DRAIN, GPIO_PULL_UP);
    hal_gpio_init(&gpio_conf);

    gpio_conf.gpio = conf->scl;
    hal_gpio_init(&gpio_conf);
}

void hal_i2c_init(i2c_conf_t *conf)
{
    BW_ASSERT(conf->dnf < 16, "Invalid DNF: %d (Expected 0-15)", conf->dnf);
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Enable irq
    if (conf->i2c == I2C1)
    {
        NVIC_EnableIRQ(I2C1_EV_IRQn);
        NVIC_EnableIRQ(I2C1_ER_IRQn);
        NVIC_SetPriority(I2C1_EV_IRQn, conf->irq_priority);
        NVIC_SetPriority(I2C1_ER_IRQn, conf->irq_priority);
    }
    else if (conf->i2c == I2C3)
    {
        NVIC_EnableIRQ(I2C3_EV_IRQn);
        NVIC_EnableIRQ(I2C3_ER_IRQn);
        NVIC_SetPriority(I2C3_EV_IRQn, conf->irq_priority);
        NVIC_SetPriority(I2C3_ER_IRQn, conf->irq_priority);
    }

    // Configure I2C clock
    i2c_clock_init(conf->i2c);

    // SDA and SCL gpio init
    i2c_gpio_init(conf);

    // Clear the PE bit
    reg_clear_mask(&conf->i2c->CR1, I2C_CR1_PE_Msk);

    // Configure Analog and digital filter
    reg_set_field(&conf->i2c->CR1, I2C_CR1_DNF_Pos, 4, conf->dnf);

    // Configure the timing
    switch (conf->speed)
    {
    case I2C_SPEED_STANDARD:
    {
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Pos, 4, 0x3);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Pos, 8, 0x13);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Pos, 8, 0xF);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Pos, 4, 0x2);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Pos, 4, 0x4);
        break;
    }
    case I2C_SPEED_FAST:
    {
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Pos, 4, 0x1);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Pos, 8, 0x9);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Pos, 8, 0x3);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Pos, 4, 0x2);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Pos, 4, 0x3);
        break;
    }
    case I2C_SPEED_FAST_PLUS:
    {
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Pos, 4, 0x0);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Pos, 8, 0x4);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Pos, 8, 0x2);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Pos, 4, 0x0);
        reg_set_field(&conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Pos, 4, 0x2);
        break;
    }
    }

    // Configure NOSTRETCH
    reg_clear_mask(&conf->i2c->CR1, I2C_CR1_NOSTRETCH_Msk);

    // Set PE bit
    reg_set_mask(&conf->i2c->CR1, I2C_CR1_PE_Msk);
}

void hal_i2c_reset(I2C_TypeDef *i2c)
{
    // 3 step process to reset
    reg_clear_mask(&i2c->CR1, I2C_CR1_PE_Msk);
    if (!reg_get_bit(&i2c->CR1, I2C_CR1_PE_Pos))
    {
        reg_set_mask(&i2c->CR1, I2C_CR1_PE_Msk);
    }
}

static void i2c_configure_reload(i2c_handle_t *handle)
{
    if (handle->len > 255)
    {
        reg_set_field(&handle->i2c->CR2, I2C_CR2_NBYTES_Pos, 8, 255);
        reg_set_mask(&handle->i2c->CR2, I2C_CR2_RELOAD_Msk);
        reg_clear_mask(&handle->i2c->CR2, I2C_CR2_AUTOEND_Msk);
    }
    else
    {
        reg_set_field(&handle->i2c->CR2, I2C_CR2_NBYTES_Pos, 8, handle->len);
        reg_clear_mask(&handle->i2c->CR2, I2C_CR2_RELOAD_Msk);
        if (handle->repeat)
        {
            reg_clear_mask(&handle->i2c->CR2, I2C_CR2_AUTOEND_Msk);
        }
        else
        {
            reg_set_mask(&handle->i2c->CR2, I2C_CR2_AUTOEND_Msk);
        }
    }
}

void hal_i2c_receive(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_RX;
    handle->total = handle->len;
    if (handle->len == 0)
    {
        return;
    }

    // Store the handles
    if (handle->i2c == I2C1)
    {
        g_i2c_handles[I2C_PERIPH_1] = handle;
    }
    else if (handle->i2c == I2C3)
    {
        g_i2c_handles[I2C_PERIPH_3] = handle;
    }

    // Clear old fields
    handle->i2c->CR2 = 0;

    // Set 7 bit address
    reg_clear_mask(&handle->i2c->CR2, I2C_CR2_ADD10_Msk);                      // 0 = 7 bit address
    reg_set_field(&handle->i2c->CR2, I2C_CR2_SADD_Pos + 1, 7, handle->addr);  // left shift by one

    // Set RXIE for interrupt
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_NACKIE_Msk |
                                    I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);

    // Set transfer direction (1 = read)
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_RD_WRN_Msk);

    // Configure NBYTES, RELOAD and AUTOEND
    i2c_configure_reload(handle);

    // Set start bit
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
}

void hal_i2c_transmit(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_TX;
    handle->total = handle->len;
    if (handle->len == 0)
    {
        return;
    }

    // Store the handles
    if (handle->i2c == I2C1)
    {
        g_i2c_handles[I2C_PERIPH_1] = handle;
    }
    else if (handle->i2c == I2C3)
    {
        g_i2c_handles[I2C_PERIPH_3] = handle;
    }

    // Clear old fields
    handle->i2c->CR2 = 0;

    // Set 7 bit address
    reg_clear_mask(&handle->i2c->CR2, I2C_CR2_ADD10_Msk);                      // 0 = 7 bit address
    reg_set_field(&handle->i2c->CR2, I2C_CR2_SADD_Pos + 1, 7, handle->addr);  // left shift by one

    // Set TXIE for interrupt
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_NACKIE_Msk |
                                    I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);

    // Set transfer direction (0 = write)
    reg_clear_mask(&handle->i2c->CR2, I2C_CR2_RD_WRN_Msk);

    // Configure NBYTES, RELOAD and AUTOEND
    i2c_configure_reload(handle);

    // Set start bit
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
}

void hal_i2c_ev_isr(i2c_perip_t type)
{
    i2c_handle_t *handle = g_i2c_handles[type];
    I2C_TypeDef *i2c = handle->i2c;

    if (reg_get_bit(&i2c->ISR, I2C_ISR_STOPF_Pos) || reg_get_bit(&i2c->ISR, I2C_ISR_NACKF_Pos))
    {
        // Clear all interrupt bits
        reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk |
                                          I2C_CR1_NACKIE_Msk | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_NACKF_Pos))
    {
        reg_set_bit(&i2c->ICR, I2C_ICR_NACKCF_Pos);
        handle->on_complete(STATUS_I2C_NACKF);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_STOPF_Pos))
    {
        reg_set_bit(&i2c->ICR, I2C_ICR_STOPCF_Pos);
        handle->on_complete(STATUS_OK);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_RXNE_Pos))
    {
        uint16_t idx = handle->total - handle->len--;
        *(handle->buf + idx) = i2c->RXDR;
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_TXIS_Pos))
    {
        uint16_t idx = handle->total - handle->len--;
        i2c->TXDR = *(handle->buf + idx);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_TC_Pos))  // only happens during repeated start
    {
        // clear trasmission and receiver interrupts
        reg_clear_mask(&handle->i2c->CR1, I2C_CR1_TXIE_Msk);
        reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk);

        // callback to continue the repeated start
        handle->on_complete(STATUS_I2C_REPEATED_START);

        // Set transfer direction (1 = read)
        reg_set_field(&handle->i2c->CR2, I2C_CR2_RD_WRN_Pos, 1, handle->type);

        // Configure NBYTES, RELOAD and AUTOEND
        i2c_configure_reload(handle);

        // Set start bit
        reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_TCR_Pos))
    {
        i2c_configure_reload(handle);
        return;
    }
}

void hal_i2c_er_isr(i2c_perip_t type)
{
    i2c_handle_t *handle = g_i2c_handles[type];

    reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk |
                                      I2C_CR1_NACKIE_Msk | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);

    handle->on_complete(STATUS_I2C_ERR);
}

void hal_i2c_deinit(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)
    {
        NVIC_DisableIRQ(I2C1_EV_IRQn);
        NVIC_DisableIRQ(I2C1_ER_IRQn);
    }
    else if (i2c == I2C3)
    {
        NVIC_DisableIRQ(I2C3_EV_IRQn);
        NVIC_DisableIRQ(I2C3_ER_IRQn);
    }
    reg_clear_mask(&i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk |
                              I2C_CR1_NACKIE_Msk | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);
}
