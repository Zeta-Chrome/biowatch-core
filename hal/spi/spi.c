#include "hal/spi/spi.h"
#include "utils/status.h"
#include "utils/assert.h"
#include "hal/gpio/gpio.h"
#include "hal/gpio/gpio_types.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"

static spi_handle_t *g_spi_handles[MAX_SPI_PERIPHERALS];

static void spi_clock_init(SPI_TypeDef *spi)
{
    if (spi == SPI1)
    {
        reg_set_bit(&RCC->APB2ENR, RCC_APB2ENR_SPI1EN_Pos);
    }
    else if (spi == SPI2)
    {
        reg_set_bit(&RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN_Pos);
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

void hal_spi_init(spi_conf_t *conf)
{
    BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)", conf->irq_priority);

    // Clock init
    spi_clock_init(conf->spi);

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
        reg_set_bit(&conf->spi->CR1, SPI_CR1_BIDIMODE_Pos);
        reg_set_field(&conf->spi->CR1, SPI_CR1_BIDIOE_Pos, 1, conf->bidioe);
        break;
    case SPI_MODE_SIMPLEX:
        reg_set_bit(&conf->spi->CR1, SPI_CR1_RXONLY_Pos);
        break;
    case SPI_MODE_FULL_DUPLEX:
        break;
    };

    // Configure frame format
    reg_set_field(&conf->spi->CR1, SPI_CR1_LSBFIRST_Pos, 1, conf->frame_format);

    // Configure NSS control (software master control)
    reg_set_mask(&conf->spi->CR1, SPI_CR1_SSM_Msk | SPI_CR1_SSI_Msk);

    // Configure master pin
    reg_set_bit(&conf->spi->CR1, SPI_CR1_MSTR_Pos);
}

void hal_spi_transact(spi_handle_t *handle)
{
    BW_ASSERT(handle->write_sz >= 4 && handle->write_sz <= 16,
              "Invalid Data Len: %d (Expected range 4-16)", handle->write_sz);

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
    reg_set_field(&handle->spi->CR2, SPI_CR2_DS_Pos, 4, handle->write_sz);

    // Set SPI MOTOROLA mode
    reg_clear_bit(&handle->spi->CR2, SPI_CR2_FRF_Pos);

    // Set FRXTH
    reg_set_field(&handle->spi->CR2, SPI_CR2_FRXTH_Pos, 1, handle->frxth);

    // Enable interrupts
    reg_set_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Pos);

    // Enable peripheral
    reg_set_bit(&handle->spi->CR1, SPI_CR1_SPE_Pos);
}

void hal_spi_isr(spi_perip_t type)
{
    spi_handle_t *handle = g_spi_handles[type];

    if (reg_get_bit(&handle->spi->SR, SPI_SR_TXE_Pos))
    {
        *(__IO uint8_t *)&handle->spi->DR = *(handle->tx_buf + handle->tx_count++);
        if (handle->write_sz > 8)
        {
            *(__IO uint8_t *)&handle->spi->DR = *(handle->tx_buf + handle->tx_count++);
        }
        return;
    }

    if (reg_get_bit(&handle->spi->SR, SPI_SR_RXNE_Pos))
    {
        // Only one byte for SPI_FRXTH_GE_QUARTER
        *(handle->rx_buf + handle->rx_count++) = handle->spi->DR;
        if (handle->frxth == SPI_FRXTH_GE_HALF)
        {
            *(handle->rx_buf + handle->rx_count++) = handle->spi->DR;
        }
        return;
    }

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
        handle->on_callback(STATUS_SPI_OVR);
        return;
    }

    if (reg_get_bit(&handle->spi->SR, SPI_SR_MODF_Pos))
    {
        handle->spi->CR1 = handle->spi->CR1;  // Write to the CR1 to clear MODF bit
        reg_clear_mask(&handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
        reg_set_mask(&handle->spi->CR1, SPI_CR1_MSTR_Msk | SPI_CR1_SPE_Msk);
        handle->on_callback(STATUS_SPI_MODF);
        return;
    }
}

void hal_spi_deinit(SPI_TypeDef *spi)
{
    if (spi == SPI1)
    {
        NVIC_DisableIRQ(SPI1_IRQn);
    }
    else if (spi == SPI2)
    {
        NVIC_DisableIRQ(SPI2_IRQn);
    }

    if (reg_get_bit(&spi->CR1, SPI_CR1_BIDIMODE_Pos) && !reg_get_bit(&spi->CR1, SPI_CR1_BIDIOE_Pos))
    {
        reg_clear_bit(&spi->CR1, SPI_CR1_SPE_Pos);
        while (reg_get_bit(&spi->SR, SPI_SR_BSY_Pos));
    }
    else
    {
        while (reg_get_field(&spi->SR, SPI_SR_FTLVL_Pos, 2));
        while (reg_get_bit(&spi->SR, SPI_SR_BSY_Pos));
        reg_clear_bit(&spi->CR1, SPI_CR1_SPE_Pos);
    }

    // clean up the rx buffer
    while (reg_get_bit(&spi->SR, SPI_SR_FRLVL_Pos) != 0)
    {
        (void)spi->DR;
    }
}
