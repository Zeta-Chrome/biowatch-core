#include "drivers/dma/dma.h"
#include "drivers/gpio/gpio.h"
#include "drivers/gpio/gpio_types.h"
#include "lib/assert.h"
#include "lib/status.h"
#include "lib/utils.h"
#include "spi.h"
#include "stm32wb55xx.h"

#define NULL ((void *)0)

#define SPI_RX_DMA DMA1
#define SPI_RX_DMA_CH_NO 1
#define SPI1_RX_DMAREQ_ID 6
#define SPI2_RX_DMAREQ_ID 8

#define SPI_TX_DMA DMA1
#define SPI_TX_DMA_CH_NO 2
#define SPI1_TX_DMAREQ_ID 7
#define SPI2_TX_DMAREQ_ID 9

static struct spi_handle *g_spi_handles[MAX_SPI_PERIPHERALS];
static struct dma_handle g_rx_dma_h;
static struct dma_handle g_tx_dma_h;
static uint16_t g_rx_dummy_val;
static uint16_t g_tx_dummy_val = 0xFFFF;

static void spi_clock_init(SPI_TypeDef *spi)
{
	if (spi == SPI1)
		SET_FIELD(RCC->APB2ENR, RCC_APB2ENR_SPI1EN_Msk);
	else if (spi == SPI2)
		SET_FIELD(RCC->APB1ENR1, RCC_APB1ENR1_SPI2EN_Msk);
}

static void spi_gpio_init(struct spi_conf *conf)
{
	struct gpio_conf gpio_conf =
		gpio_conf_af(conf->miso, GPIO_AF5, GPIO_TYPE_PUSH_PULL, GPIO_PULL_NONE);
	gpio_init(&gpio_conf);

	gpio_conf.gpio = conf->mosi;
	gpio_init(&gpio_conf);

	gpio_conf.gpio = conf->sck;
	gpio_init(&gpio_conf);
}

static void spi_hw_init(struct spi_conf *conf, struct spi_handle *handle)
{
	BW_ASSERT(conf->irq_priority < 16, "Invalid IRQ Priority %d (Expected 0-15)",
			  conf->irq_priority);

	handle->spi = conf->spi;
	handle->perip = conf->spi == SPI1 ? SPI_PERIPH_1 : SPI_PERIPH_2;

	spi_clock_init(conf->spi);
	spi_gpio_init(conf);
	MODIFY_FIELD(conf->spi->CR1, SPI_CR1_BR_Msk, SPI_CR1_BR_Pos, conf->baud_rate);
	MODIFY_BIT(conf->spi->CR1, SPI_CR1_CPOL_Pos, conf->cpol);
	MODIFY_BIT(conf->spi->CR1, SPI_CR1_CPHA_Pos, conf->cpha);

	switch (conf->mode) {
	case SPI_MODE_HALF_DUPLEX:
		SET_FIELD(conf->spi->CR1, SPI_CR1_BIDIMODE_Msk);
		MODIFY_BIT(conf->spi->CR1, SPI_CR1_BIDIOE_Pos, conf->bidioe);
		break;
	case SPI_MODE_SIMPLEX:
		SET_FIELD(conf->spi->CR1, SPI_CR1_RXONLY_Msk);
		break;
	case SPI_MODE_FULL_DUPLEX:
		break;
	};

	MODIFY_BIT(conf->spi->CR1, SPI_CR1_LSBFIRST_Pos, conf->frame_format);
	SET_FIELD(conf->spi->CR1, SPI_CR1_SSM_Msk | SPI_CR1_SSI_Msk);
}

void spi_init(struct spi_conf *conf, struct spi_handle *handle)
{
	spi_hw_init(conf, handle);

	if (conf->spi == SPI1) {
		NVIC_EnableIRQ(SPI1_IRQn);
		NVIC_SetPriority(SPI1_IRQn, conf->irq_priority);
	} else if (conf->spi == SPI2) {
		NVIC_EnableIRQ(SPI2_IRQn);
		NVIC_SetPriority(SPI2_IRQn, conf->irq_priority);
	}
}

void spi_init_dma(struct spi_conf *conf, struct spi_handle *handle)
{
	spi_init(conf, handle);

	struct dma_conf tx_dma_conf = { .dma = SPI_TX_DMA,
									.ch_no = SPI_TX_DMA_CH_NO,
									.priority = DMA_PL_VERY_HIGH,
									.dmamux = { .dmareq_id = conf->spi == SPI1 ? SPI1_TX_DMAREQ_ID :
																				 SPI2_TX_DMAREQ_ID,
												.sync_pol = false },
									.irq_priority = conf->irq_priority };
	dma_init(&tx_dma_conf, &g_tx_dma_h);

	struct dma_conf rx_dma_conf = { .dma = SPI_RX_DMA,
									.ch_no = SPI_RX_DMA_CH_NO,
									.priority = DMA_PL_VERY_HIGH,
									.dmamux = { .dmareq_id = conf->spi == SPI1 ? SPI1_RX_DMAREQ_ID :
																				 SPI2_RX_DMAREQ_ID,
												.sync_pol = false },
									.irq_priority = conf->irq_priority };
	dma_init(&rx_dma_conf, &g_rx_dma_h);
}

static void spi_prep_transaction(struct spi_handle *handle)
{
	BW_ASSERT(handle->data_sz >= 4 && handle->data_sz <= 16,
			  "Invalid Data Len: %d (Expected range 4-16)", handle->data_sz);

	handle->tx_count = handle->rx_count = 0;
	g_spi_handles[handle->perip] = handle;

	SET_FIELD(handle->spi->CR1, SPI_CR1_MSTR_Msk);
	CLEAR_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk);
	CLEAR_FIELD(handle->spi->CR2, SPI_CR2_TXDMAEN_Msk | SPI_CR2_RXDMAEN_Msk | SPI_CR2_TXEIE_Msk |
									  SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
	MODIFY_FIELD(handle->spi->CR2, SPI_CR2_DS_Msk, SPI_CR2_DS_Pos, handle->data_sz - 1);
	CLEAR_FIELD(handle->spi->CR2, SPI_CR2_FRF_Msk);
	MODIFY_BIT(handle->spi->CR2, SPI_CR2_FRXTH_Pos, handle->data_sz <= 8);
	SET_FIELD(handle->spi->CR2, SPI_CR2_ERRIE_Msk);
}

static inline void spi_write_buf(struct spi_handle *handle)
{
	if (handle->tx_count - handle->rx_count >= 1)
		return;

	if (handle->data_sz > 8) {
		uint16_t val = handle->tx_buf ? *((uint16_t *)(handle->tx_buf + handle->tx_count)) :
										g_tx_dummy_val;
		*(__IO uint16_t *)&handle->spi->DR = val;
		handle->tx_count += 2;
	} else {
		uint8_t val = handle->tx_buf ? *(handle->tx_buf + handle->tx_count++) : g_tx_dummy_val;
		*(__IO uint8_t *)&handle->spi->DR = val;
	}
}

static inline void spi_read_buf(struct spi_handle *handle)
{
	if (handle->data_sz > 8) {
		if (handle->rx_buf)
			*((uint16_t *)(handle->rx_buf + handle->rx_count)) = *(__IO uint16_t *)&handle->spi->DR;

		handle->rx_count += 2;
	} else {
		if (handle->rx_buf)
			*(handle->rx_buf + handle->rx_count) = *(__IO uint8_t *)&handle->spi->DR;

		handle->rx_count++;
	}

	if (handle->rx_count >= handle->len) {
		// Busy wait for completion
		while (handle->spi->SR & SPI_SR_FTLVL_Msk)
			;
		while (handle->spi->SR & SPI_SR_BSY_Msk)
			;

		CLEAR_FIELD(handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
		handle->callback(STATUS_OK, handle->user_data);
	}
}

void spi_isr(enum spi_perip type)
{
	struct spi_handle *handle = g_spi_handles[type];
	uint32_t sr = handle->spi->SR;

	if (sr & SPI_SR_OVR_Msk) {
		(void)handle->spi->DR;
		(void)handle->spi->SR;
		while (handle->spi->SR & SPI_SR_FRLVL_Msk)
			(void)handle->spi->DR;

		CLEAR_FIELD(handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
		CLEAR_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk);
		handle->callback(STATUS_SPI_OVR, handle->user_data);
		return;
	}

	if (sr & SPI_SR_MODF_Msk) {
		handle->spi->CR1 = handle->spi->CR1;
		CLEAR_FIELD(handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);
		CLEAR_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk); // Write CR1 to clear the MODF
		handle->callback(STATUS_SPI_MODF, handle->user_data);
		return;
	}

	if (sr & SPI_SR_RXNE_Msk) {
		spi_read_buf(handle);
		return;
	}

	if (sr & SPI_SR_TXE_Msk) {
		spi_write_buf(handle);
		return;
	}
}

void spi_transact(struct spi_handle *handle)
{
	spi_prep_transaction(handle);
	SET_FIELD(handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk);
	SET_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk);
}

void spi_isr_dma(enum bw_status status, void *user_data)
{
	struct spi_handle *handle = (struct spi_handle *)user_data;

	// Busy wait for completion
	while (handle->spi->SR & SPI_SR_FTLVL_Msk)
		;
	while (handle->spi->SR & SPI_SR_BSY_Msk)
		;

	CLEAR_FIELD(handle->spi->CR2, SPI_CR2_TXEIE_Msk | SPI_CR2_RXNEIE_Msk | SPI_CR2_ERRIE_Msk);

	if (status == STATUS_DMA_TC)
		handle->callback(STATUS_OK, handle->user_data);
	else
		handle->callback(status, handle->user_data);
}

void spi_transact_dma(struct spi_handle *handle)
{
	spi_prep_transaction(handle);

	enum dma_sz dma_sz = (handle->data_sz > 8) ? DMA_SZ_16 : DMA_SZ_8;
	bool rx_is_null = (handle->rx_buf == NULL);
	g_rx_dma_h.user_data = handle;
	g_rx_dma_h.callback = spi_isr_dma;

	struct dma_transfer rx_dma_trnf = {
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
	dma_start(&g_rx_dma_h, &rx_dma_trnf);

	bool tx_is_null = (handle->tx_buf == NULL);
	g_tx_dma_h.user_data = handle;
	g_tx_dma_h.callback = NULL;

	struct dma_transfer tx_dma_trnf = {
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
	dma_start(&g_tx_dma_h, &tx_dma_trnf);

	SET_FIELD(handle->spi->CR2, SPI_CR2_TXDMAEN_Msk | SPI_CR2_RXDMAEN_Msk);
	SET_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk);
}

void spi_deinit(struct spi_handle *handle)
{
	if (handle->spi == SPI1)
		NVIC_DisableIRQ(SPI1_IRQn);
	else if (handle->spi == SPI2)
		NVIC_DisableIRQ(SPI2_IRQn);

	if ((handle->spi->CR1 & SPI_CR1_BIDIMODE_Msk) && !(handle->spi->CR1 & SPI_CR1_BIDIOE_Msk)) {
		CLEAR_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk);
		while (handle->spi->SR & SPI_SR_BSY_Msk)
			;
	} else {
		while (handle->spi->SR & SPI_SR_FTLVL_Msk)
			;
		while (handle->spi->SR & SPI_SR_BSY_Msk)
			;
		CLEAR_FIELD(handle->spi->CR1, SPI_CR1_SPE_Msk);
	}

	// clean up the rx buffer
	while (handle->spi->SR & SPI_SR_FRLVL_Msk)
		(void)handle->spi->DR;
}

void spi_deinit_dma(struct spi_handle *handle)
{
	dma_denit(&g_tx_dma_h);
	dma_denit(&g_rx_dma_h);
	spi_deinit(handle);
}
