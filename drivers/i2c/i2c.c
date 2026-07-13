#include "drivers/dma/dma.h"
#include "drivers/gpio/gpio.h"
#include "i2c.h"
#include "lib/assert.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"
#include <stdint.h>

#define I2C_DMA DMA1
#define I2C1_RX_DMA_CH_NO 1
#define I2C1_RX_DMAREQ_ID 10

#define I2C1_TX_DMA_CH_NO 2
#define I2C1_TX_DMAREQ_ID 11

#define I2C3_RX_DMA_CH_NO 3
#define I2C3_RX_DMAREQ_ID 12

#define I2C3_TX_DMA_CH_NO 4
#define I2C3_TX_DMAREQ_ID 13

static i2c_handle_t *g_i2c_handles[MAX_I2C_PERIPHERALS];
static dma_handle_t g_rx_dma_handles[MAX_I2C_PERIPHERALS];
static dma_handle_t g_tx_dma_handles[MAX_I2C_PERIPHERALS];
static dma_transfer_t g_trnf_conf[MAX_I2C_PERIPHERALS] = {
    [0 ... MAX_I2C_PERIPHERALS - 1] =
        {.per_sz = DMA_SZ_8, .per_incr = false, .mem_sz = DMA_SZ_8, .mem_incr = true, .circular = false, .htc_trig = false}};

static void i2c_clock_init(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)
    {
        MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_I2C1SEL_Msk, RCC_CCIPR_I2C1SEL_Pos, 0x00); // PCLK1
        SET_FIELD(RCC->APB1ENR1, RCC_APB1ENR1_I2C1EN_Msk);
    }
    else if (i2c == I2C3)
    {
        MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_I2C3SEL_Msk, RCC_CCIPR_I2C3SEL_Pos, 0x00); // PCLK1
        SET_FIELD(RCC->APB1ENR1, RCC_APB1ENR1_I2C3EN_Msk);
    }
}

static void i2c_gpio_init(i2c_conf_t *conf)
{
    gpio_conf_t gpio_conf = gpio_conf_af(conf->sda, conf->af, GPIO_TYPE_OPEN_DRAIN, GPIO_PULL_UP);
    gpio_init(&gpio_conf);

    gpio_conf.gpio = conf->scl;
    gpio_init(&gpio_conf);
}

static void i2c_hw_init(i2c_conf_t *conf, i2c_handle_t *handle)
{
    BW_ASSERT(conf->dnf < 16, "Invalid DNF: %d (Expected 0-15)", conf->dnf);
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Fill handle data
    handle->i2c = conf->i2c;
    handle->perip = conf->i2c == I2C1 ? I2C_PERIPH_1 : I2C_PERIPH_3;

    // Configure I2C clock
    i2c_clock_init(conf->i2c);

    // SDA and SCL gpio init
    i2c_gpio_init(conf);

    // Clear the PE bit
    CLEAR_FIELD(conf->i2c->CR1, I2C_CR1_PE_Msk);

    // Configure Analog and digital filter
    MODIFY_FIELD(conf->i2c->CR1, I2C_CR1_DNF_Msk, I2C_CR1_DNF_Pos, conf->dnf);

    // Configure the timing
    switch (conf->speed)
    {
    case I2C_SPEED_LOW: // 10kHz
    {
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Msk, I2C_TIMINGR_PRESC_Pos, 0x3);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Msk, I2C_TIMINGR_SCLL_Pos, 0xC7);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Msk, I2C_TIMINGR_SCLH_Pos, 0xC3);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Msk, I2C_TIMINGR_SDADEL_Pos, 0x2);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Msk, I2C_TIMINGR_SCLDEL_Pos, 0x4);
        break;
    }
    case I2C_SPEED_STANDARD: // 100kHz
    {
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Msk, I2C_TIMINGR_PRESC_Pos, 0x3);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Msk, I2C_TIMINGR_SCLL_Pos, 0x13);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Msk, I2C_TIMINGR_SCLH_Pos, 0xF);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Msk, I2C_TIMINGR_SDADEL_Pos, 0x2);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Msk, I2C_TIMINGR_SCLDEL_Pos, 0x4);
        break;
    }
    case I2C_SPEED_FAST: // 400KHz
    {
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Msk, I2C_TIMINGR_PRESC_Pos, 0x1);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Msk, I2C_TIMINGR_SCLL_Pos, 0x9);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Msk, I2C_TIMINGR_SCLH_Pos, 0x3);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Msk, I2C_TIMINGR_SDADEL_Pos, 0x2);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Msk, I2C_TIMINGR_SCLDEL_Pos, 0x3);
        break;
    }
    case I2C_SPEED_FAST_PLUS: // 1MHz
    {
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_PRESC_Msk, I2C_TIMINGR_PRESC_Pos, 0x0);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLL_Msk, I2C_TIMINGR_SCLL_Pos, 0x4);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLH_Msk, I2C_TIMINGR_SCLH_Pos, 0x2);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SDADEL_Msk, I2C_TIMINGR_SDADEL_Pos, 0x0);
        MODIFY_FIELD(conf->i2c->TIMINGR, I2C_TIMINGR_SCLDEL_Msk, I2C_TIMINGR_SCLDEL_Pos, 0x2);
        break;
    }
    }

    // Configure NOSTRETCH
    CLEAR_FIELD(conf->i2c->CR1, I2C_CR1_NOSTRETCH_Msk);

    // Set PE bit
    SET_FIELD(conf->i2c->CR1, I2C_CR1_PE_Msk);
}

void i2c_init(i2c_conf_t *conf, i2c_handle_t *handle)
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

void i2c_init_dma(i2c_conf_t *conf, i2c_handle_t *handle)
{
    i2c_init(conf, handle);

    // Transmitter DMA configuration
    dma_conf_t tx_dma_conf = {
        .dma = I2C_DMA,
        .ch_no = conf->i2c == I2C1 ? I2C1_TX_DMA_CH_NO : I2C3_TX_DMA_CH_NO,
        .priority = DMA_PL_VERY_HIGH,
        .dmamux = {.dmareq_id = conf->i2c == I2C1 ? I2C1_TX_DMAREQ_ID : I2C3_TX_DMAREQ_ID, .sync_pol = false}};
    dma_init(&tx_dma_conf, &g_tx_dma_handles[handle->perip]);

    // Reciever DMA configuration
    dma_conf_t rx_dma_conf = {
        .dma = I2C_DMA,
        .ch_no = conf->i2c == I2C1 ? I2C1_RX_DMA_CH_NO : I2C3_RX_DMA_CH_NO,
        .priority = DMA_PL_VERY_HIGH,
        .dmamux = {.dmareq_id = conf->i2c == I2C1 ? I2C1_RX_DMAREQ_ID : I2C3_RX_DMAREQ_ID, .sync_pol = false}};
    dma_init(&rx_dma_conf, &g_rx_dma_handles[handle->perip]);
}

void i2c_reset(i2c_handle_t *handle)
{
    // 3 step process to reset
    CLEAR_FIELD(handle->i2c->CR1, I2C_CR1_PE_Msk);
    if (!(handle->i2c->CR1 & I2C_CR1_PE_Msk))
    {
        SET_FIELD(handle->i2c->CR1, I2C_CR1_PE_Msk);
    }
}

void i2c_reset_dma(i2c_handle_t *handle)
{
    i2c_reset(handle);
}

static void i2c_configure_reload(volatile uint32_t *reg, i2c_handle_t *handle)
{
    if (handle->remaining > 255)
    {
        // Set NBYTES = 255 (11111111) and set auto reload
        SET_FIELD(*reg, I2C_CR2_NBYTES_Msk | I2C_CR2_RELOAD_Msk);
        CLEAR_FIELD(*reg, I2C_CR2_AUTOEND_Msk);
    }
    else
    {
        MODIFY_FIELD(*reg, I2C_CR2_NBYTES_Msk, I2C_CR2_NBYTES_Pos, handle->remaining);
        CLEAR_FIELD(*reg, I2C_CR2_RELOAD_Msk);
        // Don't autoend on repeat
        MODIFY_BIT(*reg, I2C_CR2_AUTOEND_Pos, !handle->repeat);
    }
}

static void i2c_prepare_transaction(i2c_handle_t *handle)
{
    CLEAR_FIELD(handle->i2c->CR1,
                I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_RXDMAEN_Msk | I2C_CR1_TXDMAEN_Msk);

    handle->remaining = handle->len;

    // Store the handles
    g_i2c_handles[handle->perip] = handle;

    // Clear old fields
    handle->i2c->CR2 = 0;

    // Set 7 bit address
    CLEAR_FIELD(handle->i2c->CR2, I2C_CR2_ADD10_Msk);                                     // 0 = 7 bit address
    MODIFY_FIELD(handle->i2c->CR2, I2C_CR2_SADD_Msk, I2C_CR2_SADD_Pos + 1, handle->addr); // left shift by one

    // Configure interrupts
    SET_FIELD(handle->i2c->CR1, I2C_CR1_TCIE_Msk | I2C_CR1_NACKIE_Msk | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);

    // Set transfer direction (0 = write, 1 = read)
    MODIFY_BIT(handle->i2c->CR2, I2C_CR2_RD_WRN_Pos, handle->type);

    // Configure NBYTES, RELOAD and AUTOEND
    i2c_configure_reload(&handle->i2c->CR2, handle);
}

static void i2c_start_dma(i2c_handle_t *handle)
{
    handle->remaining -= g_trnf_conf[handle->perip].data_count;
    if (handle->remaining == 0)
    {
        return;
    }

    uint32_t offset = handle->len - handle->remaining;
    g_trnf_conf[handle->perip].data_count = handle->remaining > 255 ? 255 : handle->remaining;
    g_trnf_conf[handle->perip].mem_addr = (uint32_t)(handle->buf + offset);

    if (handle->type == I2C_TYPE_RX)
    {
        g_trnf_conf[handle->perip].mode = DMA_MODE_PERI_TO_MEM;
        g_trnf_conf[handle->perip].per_addr = (uint32_t)&handle->i2c->RXDR;

        dma_start(&g_rx_dma_handles[handle->perip], &g_trnf_conf[handle->perip]);
    }
    else if (handle->type == I2C_TYPE_TX)
    {
        g_trnf_conf[handle->perip].mode = DMA_MODE_MEM_TO_PERI;
        g_trnf_conf[handle->perip].per_addr = (uint32_t)&handle->i2c->TXDR;

        dma_start(&g_tx_dma_handles[handle->perip], &g_trnf_conf[handle->perip]);
    }
}

void i2c_ev_isr(i2c_perip_t type)
{
    i2c_handle_t *handle = g_i2c_handles[type];
    I2C_TypeDef *i2c = handle->i2c;

    if (i2c->ISR & I2C_ISR_RXNE_Msk)
    {
        uint16_t idx = handle->len - handle->remaining--;
        *(handle->buf + idx) = i2c->RXDR;
        return;
    }

    if (i2c->ISR & I2C_ISR_TXIS_Msk)
    {
        uint16_t idx = handle->len - handle->remaining--;
        i2c->TXDR = *(handle->buf + idx);
        return;
    }

    if (i2c->ISR & I2C_ISR_TCR_Msk)
    {
        if (handle->dma_mode)
        {
            i2c_start_dma(handle);
        }
        i2c_configure_reload(&i2c->CR2, handle);
        return;
    }

    if (i2c->ISR & I2C_ISR_TC_Msk) // only happens during repeated start
    {
        CLEAR_FIELD(handle->i2c->CR1,
                    I2C_CR1_TXIE_Msk | I2C_CR1_RXIE_Msk | I2C_CR1_TXDMAEN_Msk | I2C_CR1_RXDMAEN_Msk | I2C_CR1_TCIE_Msk);

        // callback to continue the repeated start
        handle->callback(STATUS_I2C_REPEATED_START, handle->user_data);

        uint32_t cr2 = i2c->CR2;

        // Set 7 bit address and transfer direction (1 = read)
        MODIFY_FIELD(cr2, I2C_CR2_RD_WRN_Msk, I2C_CR2_RD_WRN_Pos, handle->type);

        // Configure NBYTES, RELOAD and AUTOEND
        handle->remaining = handle->len;
        i2c_configure_reload(&cr2, handle);

        if (handle->dma_mode)
        {
            g_trnf_conf[handle->perip].data_count = 0;
            i2c_start_dma(handle);
            uint32_t dma_msk = (handle->type == I2C_TYPE_RX) ? I2C_CR1_RXDMAEN_Msk : I2C_CR1_TXDMAEN_Msk;
            SET_FIELD(i2c->CR1, dma_msk);
        }
        else
        {
            uint32_t ie_msk = (handle->type == I2C_TYPE_RX) ? I2C_CR1_RXIE_Msk : I2C_CR1_TXIE_Msk;
            SET_FIELD(i2c->CR1, ie_msk);
        }

        SET_FIELD(cr2, I2C_CR2_START_Msk);

        // Set start bit
        WRITE_FIELD(i2c->CR2, cr2);

        return;
    }

    if ((i2c->ISR & I2C_ISR_STOPF_Msk) || (i2c->ISR & I2C_ISR_NACKF_Msk))
    {
        // Clear all interrupt bits
        CLEAR_FIELD(handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_NACKIE_Msk
                                          | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);
    }

    if (i2c->ISR & I2C_ISR_NACKF_Msk)
    {
        SET_FIELD(i2c->ICR, I2C_ICR_NACKCF_Msk | I2C_ICR_STOPCF_Msk);
        handle->callback(STATUS_I2C_NACKF, handle->user_data);
        return;
    }

    if (i2c->ISR & I2C_ISR_STOPF_Msk)
    {
        SET_FIELD(i2c->ICR, I2C_ICR_STOPCF_Msk);
        handle->callback(STATUS_OK, handle->user_data);
        return;
    }
}

void i2c_er_isr(i2c_perip_t type)
{
    i2c_handle_t *handle = g_i2c_handles[type];

    CLEAR_FIELD(handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_NACKIE_Msk
                                      | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);

    handle->callback(STATUS_I2C_ERR, handle->user_data);
}

void i2c_isr_dma(bw_status_t status, void *user_data)
{
    i2c_handle_t *handle = (i2c_handle_t *)user_data;

    if (status == STATUS_DMA_TERR)
    {
        handle->callback(status, handle->user_data);
        return;
    }
}

void i2c_receive(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_RX;
    handle->dma_mode = false;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure interrupts
    SET_FIELD(handle->i2c->CR1, I2C_CR1_RXIE_Msk);

    // Set start bit
    SET_FIELD(handle->i2c->CR2, I2C_CR2_START_Msk);
}

void i2c_receive_dma(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_RX;
    handle->dma_mode = true;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure DMA
    g_rx_dma_handles[handle->perip].user_data = handle;
    g_rx_dma_handles[handle->perip].callback = i2c_isr_dma;

    i2c_start_dma(handle);

    // Enable dma request line
    SET_FIELD(handle->i2c->CR1, I2C_CR1_RXDMAEN_Msk);

    // Set start bit
    SET_FIELD(handle->i2c->CR2, I2C_CR2_START_Msk);
}

void i2c_transmit(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_TX;
    handle->dma_mode = false;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure interrupts
    SET_FIELD(handle->i2c->CR1, I2C_CR1_TXIE_Msk);

    // Set start bit
    SET_FIELD(handle->i2c->CR2, I2C_CR2_START_Msk);
}

void i2c_transmit_dma(i2c_handle_t *handle)
{
    handle->type = I2C_TYPE_TX;
    handle->dma_mode = true;
    if (handle->len == 0)
    {
        return;
    }

    // Prepare transaction
    i2c_prepare_transaction(handle);

    // Configure DMA
    g_tx_dma_handles[handle->perip].user_data = handle;
    g_tx_dma_handles[handle->perip].callback = i2c_isr_dma;

    i2c_start_dma(handle);

    // Enable dma request line
    SET_FIELD(handle->i2c->CR1, I2C_CR1_TXDMAEN_Msk);

    // Set start bit
    SET_FIELD(handle->i2c->CR2, I2C_CR2_START_Msk);
}

void i2c_deinit(i2c_handle_t *handle)
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
    CLEAR_FIELD(handle->i2c->CR1, I2C_CR1_RXIE_Msk | I2C_CR1_TXIE_Msk | I2C_CR1_TCIE_Msk | I2C_CR1_NACKIE_Msk
                                      | I2C_CR1_STOPIE_Msk | I2C_CR1_ERRIE_Msk);
    CLEAR_FIELD(handle->i2c->CR1, I2C_CR1_PE_Msk);
}

void i2c_deinit_dma(i2c_handle_t *handle)
{
    dma_denit(&g_rx_dma_handles[handle->perip]);
    dma_denit(&g_tx_dma_handles[handle->perip]);
    i2c_deinit(handle);
}
