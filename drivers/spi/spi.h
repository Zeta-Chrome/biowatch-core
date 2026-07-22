#ifndef DRIVER_SPI_H
#define DRIVER_SPI_H

#include "drivers/gpio/gpio_types.h"
#include "lib/status.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

typedef void (*spi_callback_t)(enum bw_status, void *);

enum spi_cpol { SPI_CLOCK_POLARITY_LOW, SPI_CLOCK_POLARITY_HIGH };

enum spi_cpha { SPI_CLOCK_PHASE_LEADING, SPI_CLOCK_PHASE_TRAILING };

enum spi_br {
	SPI_BAUD_RATE_DIV_2,
	SPI_BAUD_RATE_DIV_4,
	SPI_BAUD_RATE_DIV_8,
	SPI_BAUD_RATE_DIV_16,
	SPI_BAUD_RATE_DIV_32,
	SPI_BAUD_RATE_DIV_64,
	SPI_BAUD_RATE_DIV_128,
	SPI_BAUD_RATE_DIV_256
};

enum spi_mode { SPI_MODE_FULL_DUPLEX, SPI_MODE_HALF_DUPLEX, SPI_MODE_SIMPLEX };

enum spi_bidioe { SPI_BIDIOE_RECEIVE_ONLY, SPI_BIDIOE_TRANSMIT_ONLY };

enum spi_format { SPI_FRAME_FORMAT_MSBFIRST, SPI_FRAME_FORMAT_LSBFIRST };

enum spi_perip { SPI_PERIPH_1, SPI_PERIPH_2, MAX_SPI_PERIPHERALS };

struct spi_conf {
	SPI_TypeDef *spi;
	struct gpio mosi;
	struct gpio miso;
	struct gpio sck;
	enum spi_br baud_rate;
	enum spi_cpol cpol;
	enum spi_cpha cpha;
	enum spi_mode mode;
	enum spi_bidioe bidioe;
	enum spi_format frame_format;
	uint8_t irq_priority; // 0 - 15
};

struct spi_handle {
	SPI_TypeDef *spi;
	enum spi_perip perip;
	uint8_t data_sz; // 4-16
	uint8_t *rx_buf;
	uint8_t rx_count;
	uint8_t *tx_buf;
	uint16_t tx_count;
	uint16_t len; // total bytes to be transacted
	void *user_data;
	spi_callback_t callback;
};

void spi_init(struct spi_conf *conf, struct spi_handle *handle);
void spi_init_dma(struct spi_conf *conf, struct spi_handle *handle);
void spi_transact(struct spi_handle *handle);
void spi_transact_dma(struct spi_handle *handle);
void spi_deinit(struct spi_handle *handle);
void spi_deinit_dma(struct spi_handle *handle);

#endif
