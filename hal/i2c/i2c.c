#include "hal/i2c/i2c.h"
#include "dma/dma.h"
#include "hal/gpio/gpio.h"
#include "hal/gpio/gpio_types.h"
#include "hal/reg.h"
#include "utils/status.h"
#include "utils/assert.h"
#include "stm32wb55xx.h"
#include <stdint.h>

#define NULL ((void *)0)

#define I2C_RX_DMA DMA2
#define I2C_RX_DMA_CH_NO 1
#define I2C1_RX_DMAREQ_ID 31
#define I2C3_RX_DMAREQ_ID 33

#define I2C_TX_DMA DMA2
#define I2C_TX_DMA_CH_NO 2
#define I2C1_TX_DMAREQ_ID 32
#define I2C3_TX_DMAREQ_ID 34

static i2c_handle_t *g_i2c_handles[MAX_I2C_PERIPHERALS];
static dma_handle_t g_rx_dma_h;
static dma_handle_t g_tx_dma_h;
static dma_transfer_t g_trnf_conf = {.per_sz = DMA_SZ_8,
                                     .per_incr = false,
                                     .mem_sz = DMA_SZ_8,
                                     .mem_incr = true,
                                     .circular = false,
                                     .htc_trig = false};

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

static void i2c_hw_init(i2c_conf_t *conf, i2c_handle_t *handle)
{
    BW_ASSERT(conf->dnf < 16, "Invalid DNF: %d (Expected 0-15)", conf->dnf);
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Fill handle data
    handle->i2c = conf->i2c;

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

void hal_i2c_init(i2c_conf_t *conf, i2c_handle_t *handle)
{
    i2c_hw_init(conf, handle);

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
}

void hal_i2c_init_dma(i2c_conf_t *conf, i2c_handle_t *handle)
{
    i2c_hw_init(conf, handle);

    // Transmitter DMA configuration
    dma_conf_t tx_dma_conf = {
    .dma = I2C_TX_DMA,
    .ch_no = I2C_TX_DMA_CH_NO,
    .priority = DMA_PL_VERY_HIGH,
    .dmamux = {.dmareq_id = conf->i2c == I2C1 ? I2C1_TX_DMAREQ_ID : I2C3_TX_DMAREQ_ID,
               .sync_pol = false}};
    hal_dma_init(&tx_dma_conf, &g_tx_dma_h);

    // Reciever DMA configuration
    dma_conf_t rx_dma_conf = {
    .dma = I2C_RX_DMA,
    .ch_no = I2C_RX_DMA_CH_NO,
    .priority = DMA_PL_VERY_HIGH,
    .dmamux = {.dmareq_id = conf->i2c == I2C3 ? I2C1_RX_DMAREQ_ID : I2C3_RX_DMAREQ_ID,
               .sync_pol = false}};
    hal_dma_init(&rx_dma_conf, &g_rx_dma_h);
}

void hal_i2c_reset(i2c_handle_t *handle)
{
    // 3 step process to reset
    reg_clear_mask(&handle->i2c->CR1, I2C_CR1_PE_Msk);
    if (!reg_get_bit(&handle->i2c->CR1, I2C_CR1_PE_Pos))
    {
        reg_set_mask(&handle->i2c->CR1, I2C_CR1_PE_Msk);
    }
}

void hal_i2c_reset_dma(i2c_handle_t *handle)
{
    hal_i2c_reset(handle);
}

static void i2c_configure_reload(i2c_handle_t *handle)
{
    if (handle->remaining > 255)
    {
        reg_set_field(&handle->i2c->CR2, I2C_CR2_NBYTES_Pos, 8, 255);
        reg_set_mask(&handle->i2c->CR2, I2C_CR2_RELOAD_Msk);
        reg_clear_mask(&handle->i2c->CR2, I2C_CR2_AUTOEND_Msk);
    }
    else
    {
        reg_set_field(&handle->i2c->CR2, I2C_CR2_NBYTES_Pos, 8, handle->remaining);
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

static void i2c_prepare_transaction(i2c_handle_t *handle)
{
    handle->remaining = handle->len;

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
    reg_clear_mask(&handle->i2c->CR2, I2C_CR2_ADD10_Msk);                     // 0 = 7 bit address
    reg_set_field(&handle->i2c->CR2, I2C_CR2_SADD_Pos + 1, 7, handle->addr);  // left shift by one

    // Configure interrupts
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_NACKIE_Msk | I2C_CR1_ERRIE_Msk);

    // Set transfer direction (0 = write, 1 = read)
    if (handle->type == I2C_TYPE_TX)
    {
        reg_clear_mask(&handle->i2c->CR2, I2C_CR2_RD_WRN_Msk);
    }
    else if (handle->type == I2C_TYPE_RX)
    {
        reg_set_mask(&handle->i2c->CR2, I2C_CR2_RD_WRN_Msk);
    }

    // Configure NBYTES, RELOAD and AUTOEND
    i2c_configure_reload(handle);
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
        handle->callback(STATUS_I2C_NACKF, handle->user_data);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_STOPF_Pos))
    {
        reg_set_bit(&i2c->ICR, I2C_ICR_STOPCF_Pos);
        handle->callback(STATUS_OK, handle->user_data);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_RXNE_Pos))
    {
        uint16_t idx = handle->len - handle->remaining--;
        *(handle->buf + idx) = i2c->RXDR;
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_TXIS_Pos))
    {
        uint16_t idx = handle->len - handle->remaining--;
        i2c->TXDR = *(handle->buf + idx);
        return;
    }

    if (reg_get_bit(&i2c->ISR, I2C_ISR_TC_Pos))  // only happens during repeated start
    {
        // callback to continue the repeated start
        handle->callback(STATUS_I2C_REPEATED_START, handle->user_data);

        if (handle->type == I2C_TYPE_RX)
        {
            reg_clear_mask(&handle->i2c->CR1, I2C_CR1_TXIE_Msk);
            reg_set_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk);
        }
        else if (handle->type == I2C_TYPE_TX)
        {
            reg_set_mask(&handle->i2c->CR1, I2C_CR1_TXIE_Msk);
            reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk);
        }

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

    handle->callback(STATUS_I2C_ERR, handle->user_data);
}

void hal_i2c_receive(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_RX;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure interrupts
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_STOPIE_Msk);

    // Set start bit
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
}

static void i2c_start_dma(i2c_handle_t *handle)
{
    g_trnf_conf.data_count = handle->remaining > 255 ? 255 : handle->remaining;
    g_trnf_conf.mem_addr = (uint32_t)handle->buf;

    if (handle->type == I2C_TYPE_TX)
    {
        g_trnf_conf.mode = DMA_MODE_PERI_TO_MEM;
        g_trnf_conf.per_addr = handle->i2c->RXDR;

        hal_dma_start(&g_rx_dma_h, &g_trnf_conf);
    }
    else if (handle->type == I2C_TYPE_RX)
    {
        g_trnf_conf.mode = DMA_MODE_MEM_TO_PERI;
        g_trnf_conf.per_addr = handle->i2c->TXDR;

        hal_dma_start(&g_tx_dma_h, &g_trnf_conf);
    }
}

void hal_i2c_isr_dma(bw_status_t status, void *user_data)
{
    i2c_handle_t *handle = (i2c_handle_t *)user_data;

    if (status != STATUS_DMA_TC)
    {
        handle->callback(status, handle->user_data);
    }

    if (reg_get_bit(&handle->i2c->ISR, I2C_ISR_STOPF_Pos))
    {
        reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXDMAEN_Msk | I2C_CR1_TXDMAEN_Msk |
                                          I2C_CR1_NACKIE_Msk | I2C_CR1_ERRIE_Msk);

        handle->callback(STATUS_OK, handle->user_data);
        return;
    }

    if (reg_get_bit(&handle->i2c->ISR, I2C_ISR_TCR_Pos))
    {
        i2c_configure_reload(handle);
        i2c_start_dma(handle);
        return;
    }

    if (reg_get_bit(&handle->i2c->ISR, I2C_ISR_TC_Pos))  // only happens during repeated setting
    {
        // callback to continue the repeated start
        handle->callback(STATUS_I2C_REPEATED_START, handle->user_data);

        if (handle->type == I2C_TYPE_RX)
        {
            reg_clear_mask(&handle->i2c->CR1, I2C_CR1_TXDMAEN_Msk);
            reg_set_mask(&handle->i2c->CR1, I2C_CR1_RXDMAEN_Msk);
        }
        else if (handle->type == I2C_TYPE_TX)
        {
            reg_set_mask(&handle->i2c->CR1, I2C_CR1_TXDMAEN_Msk);
            reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXDMAEN_Msk);
        }

        // Set transfer direction (1 = read)
        reg_set_field(&handle->i2c->CR2, I2C_CR2_RD_WRN_Pos, 1, handle->type);

        // Configure NBYTES, RELOAD and AUTOEND
        i2c_configure_reload(handle);
        i2c_start_dma(handle);

        // Set start bit
        reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
        return;
    }
}

void hal_i2c_receive_dma(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_RX;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure DMA
    g_rx_dma_h.user_data = handle;
    g_rx_dma_h.callback = hal_i2c_isr_dma;

    i2c_start_dma(handle);

    // Enable dma request line
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_RXDMAEN_Msk);

    // Set start bit
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
}

void hal_i2c_transmit(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_TX;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure interrupts
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_STOPIE_Msk);

    // Set start bit
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
}

void hal_i2c_transmit_dma(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_TX;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure DMA
    g_tx_dma_h.user_data = handle;
    g_tx_dma_h.callback = hal_i2c_isr_dma;

    i2c_start_dma(handle);

    // Enable dma request line
    reg_set_mask(&handle->i2c->CR1, I2C_CR1_TXDMAEN_Msk);

    // Set start bit
    reg_set_mask(&handle->i2c->CR2, I2C_CR2_START_Msk);
}

void hal_i2c_deinit(i2c_handle_t *handle)
{
    if (handle->i2c == I2C1)
    {
        NVIC_DisableIRQ(I2C1_EV_IRQn);
        NVIC_DisableIRQ(I2C1_ER_IRQn);
    }
    else if (handle->i2c == I2C3)
    {
        NVIC_DisableIRQ(I2C3_EV_IRQn);
        NVIC_DisableIRQ(I2C3_ER_IRQn);
    }
    reg_clear_mask(&handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk |
                                      I2C_CR1_NACKIE_Msk | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);
    reg_clear_mask(&handle->i2c->CR1, I2C_CR1_PE_Msk);
}

void hal_i2c_deinit_dma(i2c_handle_t *handle)
{
    hal_dma_denit(&g_rx_dma_h);
    hal_dma_denit(&g_tx_dma_h);
    hal_i2c_deinit(handle);
}
