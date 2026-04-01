#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "utils/status.h"
#include "hal/gpio/gpio_types.h"
#include "stm32wb55xx.h"
#include <stdbool.h>

typedef enum 
{
    SPI_CLOCK_POLARITY_LOW,
    SPI_CLOCK_POLARITY_HIGH
} spi_cpol_t;

typedef enum 
{
    SPI_CLOCK_PHASE_LEADING,
    SPI_CLOCK_PHASE_TRAILING
} spi_cpha_t;

typedef enum 
{
    SPI_BAUD_RATE_DIV_2,
    SPI_BAUD_RATE_DIV_4,
    SPI_BAUD_RATE_DIV_8,
    SPI_BAUD_RATE_DIV_16,
    SPI_BAUD_RATE_DIV_32,
    SPI_BAUD_RATE_DIV_64,
    SPI_BAUD_RATE_DIV_128,
    SPI_BAUD_RATE_DIV_256
} spi_br_t;

typedef enum 
{
    SPI_MODE_FULL_DUPLEX,
    SPI_MODE_HALF_DUPLEX,
    SPI_MODE_SIMPLEX
} spi_mode_t;

typedef enum 
{
    SPI_BIDIOE_RECEIVE_ONLY,
    SPI_BIDIOE_TRANSMIT_ONLY
} spi_bidioe_t;

typedef enum 
{
    SPI_FRAME_FORMAT_MSBFIRST,
    SPI_FRAME_FORMAT_LSBFIRST
} spi_format_t;

typedef enum 
{
    SPI_FRXTH_GE_HALF,
    SPI_FRXTH_GE_QUARTER
} spi_frxth_t;

typedef enum
{
    SPI_PERIPH_1,
    SPI_PERIPH_2,
    MAX_SPI_PERIPHERALS
} spi_perip_t;

typedef struct 
{
    SPI_TypeDef *spi;
    gpio_t mosi;
    gpio_t miso;
    gpio_t sck;
    gpio_t nss;
    spi_br_t baud_rate;
    spi_cpol_t cpol;
    spi_cpha_t cpha;
    spi_mode_t mode; 
    spi_bidioe_t bidioe; 
    spi_format_t frame_format;
} spi_conf_t;

typedef struct 
{
    SPI_TypeDef *spi;
    uint8_t write_sz; // 4-16
    spi_frxth_t frxth;
    uint8_t* rx_buf;
    uint16_t rx_count;
    uint8_t* tx_buf;
    uint16_t tx_count;
    uint16_t data_total; // total bytes to be transacted

    void (*on_callback)(bw_status_t);
} spi_handle_t;

void hal_spi_init(spi_conf_t* conf);
void hal_spi_transact(spi_handle_t* handle);
void hal_spi_isr(spi_perip_t type);
void hal_spi_deinit(SPI_TypeDef *spi);

#endif
