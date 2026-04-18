#include "spi.h"
#include "dma/dma.h"
#include "utils/status.h"
#include "utils/assert.h"
#include "gpio/gpio.h"
#include "gpio/gpio_types.h"
#include "stm32wb55xx.h"
#include "reg.h"

#define NULL ((void *)0)

#define SPI_RX_DMA DMA1
#define SPI_RX_DMA_CH_NO 1
#define SPI1_RX_DMAREQ_ID 27
#define SPI2_RX_DMAREQ_ID 29

#define SPI_TX_DMA DMA1
#define SPI_TX_DMA_CH_NO 2
#define SPI1_TX_DMAREQ_ID 28
#define SPI2_TX_DMAREQ_ID 30

static spi_handle_t *g_spi_handles[MAX_SPI_PERIPHERALS];
static dma_handle_t g_rx_dma_h;
static dma_handle_t g_tx_dma_h;
static uint16_t g_rx_dummy_val;
static uint16_t g_tx_dummy_val = 0xFFFF;

static void spi_clock_init(SPI_TypeDef *spi)
{
    if (spi == SPI1)
    {
        reg_set_mask(&RCC->APB2ENR, RCC_APB2ENR_SPI1EN_Msk);
    }
    else if (spi == SPI2)
    {
        reg_set_mask(&RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN_Msk);
    }
}

static void spi_gpio_init(spi_conf_t *conf)
{
    gpio_conf_t gpio_conf = gpio_conf_af(conf->miso, GPIO_AF5, GPIO_TYPE_PUSH_PULL, GPIO_PULL_NONE);
    hal_gpio_init(&gpio_conf);

    gpio_conf.gpio = conf->mosi;
    hal_gpio_init(&gpio_conf);

    gpio_conf.gpio = conf->sck;
    hal_gpio_init(&gpio_conf);

    gpio_conf.gpio = conf->nss;
    hal_gpio_init(&gpio_conf);
}

static void spi_hw_init(spi_conf_t *conf, spi_handle_t *handle)
{
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Configure handle
    handle->spi = conf->spi;

    // Clock init
    spi_clock_init(conf->spi);

    // GPIO init
    spi_gpio_init(conf);

    // Configure Baud rate
    reg_set_field(&conf->spi->CR1, SPI_CR1_BR_Pos, 3, conf->baud_rate);

    // Configure CPOL and CPHA
    reg_set_field(&conf->spi->CR1, SPI_CR1_CPOL_Pos, 1, conf->cpol);
    reg_set_field(&conf->spi->CR1, SPI_CR1_CPHA_Pos, 1, conf->cpha);

    // Configure full-duplex, half-duplex or simplex
    switch (conf->mode)
    {
    case SPI_MODE_HALF_DUPLEX:
        reg_set_mask(&conf->spi->CR1, SPI_CR1_BIDIMODE_Msk);
        reg_set_field(&conf->spi->CR1, SPI_CR1_BIDIOE_Pos, 1, conf->bidioe);
        break;
    case SPI_MODE_SIMPLEX:
        reg_set_mask(&conf->spi->CR1, SPI_CR1_RXONLY_Msk);
        break;
    case SPI_MODE_FULL_DUPLEX:
        break;
    };

    // Configure frame format
    reg_set_field(&conf->spi->CR1, SPI_CR1_LSBFIRST_Pos, 1, conf->frame_format);

    // Configure NSS control (software master control)
    reg_set_mask(&conf->spi->CR1, SPI_CR1_SSM_Msk | SPI_CR1_SSI_Msk);

    // Configure master pin
    reg_set_mask(&conf->spi->CR1, SPI_CR1_MSTR_Msk);
}

void hal_spi_init(spi_conf_t *conf, spi_handle_t *handle)
{
    // Hardware configuration
    spi_hw_init(conf, handle);

    // Enable IRQ
    if (conf->spi == SPI1)
    {
        NVIC_EnableIRQ(SPI1_IRQn);
        NVIC_SetPriority(SPI1_IRQn, conf->irq_priority);
    }
    else if (conf->spi == SPI2)
    {
        NVIC_EnableIRQ(SPI2_IRQn);
        NVIC_SetPriority(SPI2_IRQn, conf->irq_priority);
    }
}

void hal_spi_init_dma(spi_conf_t *conf, spi_handle_t *handle)
{
    // Hardware configuration
    spi_hw_init(conf, handle);

    // Transmitter DMA configuration
    dma_conf_t tx_dma_conf = {
    .dma = SPI_TX_DMA,
    .ch_no = SPI_TX_DMA_CH_NO,
    .priority = DMA_PL_VERY_HIGH,
    .dmamux = {.dmareq_id = conf->spi == SPI1 ? SPI1_TX_DMAREQ_ID : SPI2_TX_DMAREQ_ID,
               .sync_pol = false}};
    hal_dma_init(&tx_dma_conf, &g_tx_dma_h);

    // Reciever DMA configuration
    dma_conf_t rx_dma_conf = {
    .dma = SPI_RX_DMA,
    .ch_no = SPI_RX_DMA_CH_NO,
    .priority = DMA_PL_VERY_HIGH,
    .dmamux = {.dmareq_id = conf->spi == SPI1 ? SPI1_RX_DMAREQ_ID : SPI2_RX_DMAREQ_ID,
               .sync_pol = false}};
    hal_dma_init(&rx_dma_conf, &g_rx_dma_h);
}

static void spi_prep_transaction(spi_handle_t *handle)
{
    BW_ASSERT(handle->data_sz >= 4 && handle->data_sz <= 16,
              "Invalid Data Len: %d (Expected range 4-16)", handle->data_sz);

    // Store the handles
    if (handle->spi == SPI1)
    {
        g_spi_handles[SPI_PERIPH_1] = handle;
    }
    else if (handle->spi == SPI2)
    {
        g_spi_handles[SPI_PERIPH_2] = handle;
    }

    // Configure DS
    reg_set_field(&handle->spi->CR2, SPI_CR2_DS_Pos, 4, handle->data_sz);

    // Set SPI MOTOROLA mode
    reg_clear_mask(&handle->spi->CR2, SPI_CR2_FRF_Msk);

    // Set FRXTH
    reg_set_field(&handle->spi->CR2, SPI_CR2_FRXTH_Pos, 1, handle->data_sz > 8);

    // Enable error interrupt
    reg_set_mask(&handle->spi->CR2, SPI_CR2_ERRIE_Msk);
}

static inline void spi_write_buf(spi_handle_t *handle)
{

    if (handle->data_sz > 8)
    {
        uint16_t val = handle->tx_buf ? *((uint16_t *)(handle->tx_buf + handle->tx_count))
                                      : g_tx_dummy_val;
        *(__IO uint16_t *)&handle->spi->DR = val;
        handle->tx_count += 2;
    }
    else
    {
        uint8_t val = handle->tx_buf ? *(handle->tx_buf + handle->tx_count++) : g_tx_dummy_val;
        *(__IO uint8_t *)&handle->spi->DR = val;
    }
}

static inline void spi_read_buf(spi_handle_t *handle)
{

    if (handle->data_sz > 8)
    {
        if (handle->rx_buf)
        {
            *((uint16_t *)(handle->rx_buf + handle->rx_count)) = handle->spi->DR;
        }
        handle->rx_count += 2;
    }
    else
    {
        if (handle->rx_buf)
        {
            *(handle->rx_buf + handle->rx_count) = handle->spi->DR;
        }
        handle->rx_count++;
    }

    if (handle->rx_count > handle->len)
    {
        reg_clear_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
        handle->callback(STATUS_OK, handle->user_data);
    }
}

void hal_spi_isr(spi_perip_t type)
{
    spi_handle_t *handle = g_spi_handles[type];

    if (reg_get_bit(&handle->spi->SR, SPI_SR_OVR_Pos))
    {
        (void)handle->spi->DR;
        (void)handle->spi->SR;
        while (reg_get_bit(&handle->spi->SR, SPI_SR_FRLVL_Pos) != 0)
        {
            (void)handle->spi->DR;
        }
        reg_clear_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
        reg_clear_mask(&handle->spi->CR1, SPI_CR1_SPE_Msk);
        handle->callback(STATUS_SPI_OVR, handle->user_data);
        return;
    }

    if (reg_get_bit(&handle->spi->SR, SPI_SR_MODF_Pos))
    {
        handle->spi->CR1 = handle->spi->CR1;  // Write to the CR1 to clear MODF bit
        reg_clear_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
        reg_set_mask(&handle->spi->CR1, SPI_CR1_MSTR_Msk | SPI_CR1_SPE_Msk);
        handle->callback(STATUS_SPI_MODF, handle->user_data);
        return;
    }

    if (reg_get_bit(&handle->spi->SR, SPI_SR_TXE_Pos))
    {
        spi_write_buf(handle);
        return;
    }

    if (reg_get_bit(&handle->spi->SR, SPI_SR_RXNE_Pos))
    {
        spi_read_buf(handle);
        return;
    }
}

void hal_spi_transact(spi_handle_t *handle)
{
    // Prepare transfer
    spi_prep_transaction(handle);

    // Enable interrupts
    reg_set_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk);

    // Enable peripheral
    reg_set_mask(&handle->spi->CR1, SPI_CR1_SPE_Msk);
}

void hal_spi_isr_dma(bw_status_t status, void *user_data)
{
    spi_handle_t *handle = (spi_handle_t *)user_data;
    reg_clear_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
    reg_set_mask(&handle->spi->CR1, SPI_CR1_MSTR_Msk | SPI_CR1_SPE_Msk);

    if (status == STATUS_DMA_TC)
    {
        handle->callback(STATUS_OK, handle->user_data);
    }
    else
    {
        handle->callback(status, handle->user_data);
    }
}

void hal_spi_transact_dma(spi_handle_t *handle)
{
    // Prepare hardware common settings
    spi_prep_transaction(handle);

    // Determine Data Size for DMA (8-bit or 16-bit)
    dma_sz_t dma_sz = (handle->data_sz > 8) ? DMA_SZ_16 : DMA_SZ_8;

    // Receiver Configuration
    bool rx_is_null = (handle->rx_buf == NULL);
    g_rx_dma_h.user_data = handle;
    g_rx_dma_h.callback = hal_spi_isr_dma;

    dma_transfer_t rx_dma_trnf = {
    .data_count = handle->len,
    .mode = DMA_MODE_PERI_TO_MEM,
    .per_addr = (uint32_t)&handle->spi->DR,
    .mem_addr = rx_is_null ? (uint32_t)&g_rx_dummy_val : (uint32_t)handle->rx_buf,
    .mem_sz = dma_sz,
    .per_sz = dma_sz,
    .per_incr = false,
    .mem_incr = !rx_is_null,
    .circular = false,
    .htc_trig = false,
    };
    hal_dma_start(&g_rx_dma_h, &rx_dma_trnf);

    // Transmitter Configuration
    bool tx_is_null = (handle->tx_buf == NULL);
    g_tx_dma_h.user_data = handle;
    g_tx_dma_h.callback = NULL;

    dma_transfer_t tx_dma_trnf = {
    .data_count = handle->len,
    .mode = DMA_MODE_MEM_TO_PERI,
    .per_addr = (uint32_t)&handle->spi->DR,
    .mem_addr = tx_is_null ? (uint32_t)&g_tx_dummy_val : (uint32_t)handle->tx_buf,
    .mem_sz = dma_sz,
    .per_sz = dma_sz,
    .per_incr = false,
    .mem_incr = !tx_is_null,
    .circular = false,
    .htc_trig = false,
    };
    hal_dma_start(&g_tx_dma_h, &tx_dma_trnf);

    // Enable DMA requests in SPI
    reg_set_mask(&handle->spi->CR2, SPI_CR2_TXDMAEN_Msk | SPI_CR2_RXDMAEN_Msk);

    // Enable peripheral (starts the clock)
    reg_set_mask(&handle->spi->CR1, SPI_CR1_SPE_Msk);
}

void hal_spi_deinit(spi_handle_t *handle)
{
    if (handle->spi == SPI1)
    {
        NVIC_DisableIRQ(SPI1_IRQn);
    }
    else if (handle->spi == SPI2)
    {
        NVIC_DisableIRQ(SPI2_IRQn);
    }

    if (reg_get_bit(&handle->spi->CR1, SPI_CR1_BIDIMODE_Pos) &&
        !reg_get_bit(&handle->spi->CR1, SPI_CR1_BIDIOE_Pos))
    {
        reg_clear_mask(&handle->spi->CR1, SPI_CR1_SPE_Msk);
        while (reg_get_bit(&handle->spi->SR, SPI_SR_BSY_Pos));
    }
    else
    {
        while (reg_get_field(&handle->spi->SR, SPI_SR_FTLVL_Pos, 2));
        while (reg_get_bit(&handle->spi->SR, SPI_SR_BSY_Pos));
        reg_clear_mask(&handle->spi->CR1, SPI_CR1_SPE_Msk);
    }

    // clean up the rx buffer
    while (reg_get_bit(&handle->spi->SR, SPI_SR_FRLVL_Pos) != 0)
    {
        (void)handle->spi->DR;
    }
}

void hal_spi_deinit_dma(spi_handle_t *handle)
{
    hal_dma_denit(&g_tx_dma_h);
    hal_dma_denit(&g_rx_dma_h);
    hal_spi_deinit(handle);
}
