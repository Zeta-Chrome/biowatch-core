#ifndef HAL_DMA_H
#define HAL_DMA_H

#include <stdint.h>
#include <stdbool.h>
#include "utils/status.h"
#include "stm32wb55xx.h"

typedef enum
{
    DMA_PL_LOW,
    DMA_PL_MEDIUM,
    DMA_PL_HIGH,
    DMA_PL_VERY_HIGH
} dma_pl_t;

typedef enum
{
    DMA_SZ_8,
    DMA_SZ_16,
    DMA_SZ_32
} dma_sz_t;

typedef enum
{
    DMA_MODE_PERI_TO_MEM,
    DMA_MODE_MEM_TO_PERI,
} dma_mode_t;

typedef enum
{
    DMAMUX_SPOL_NONE,
    DMAMUX_SPOL_RISING,
    DMAMUX_SPOL_FALLING,
    DMAMUX_SPOL_BOTH
} dmamux_spol_t;

typedef struct 
{
    uint8_t dmareq_id;
    bool sync_en;
    uint8_t sync_id;
    dmamux_spol_t sync_pol;
    uint8_t num_req;
} dmamux_conf_t;

typedef struct
{
    DMA_TypeDef *dma;
    uint8_t dma_ch_no;
    dma_pl_t priority;
    dmamux_conf_t dmamux;
} dma_conf_t;

typedef struct
{
    uint16_t data_count;
    dma_mode_t mode;
    uint32_t per_addr;
    dma_sz_t per_sz;
    bool per_incr_mode;
    uint32_t mem_addr;
    dma_sz_t mem_sz;
    bool mem_incr_mode;
    bool circular;
    bool htc_trig;
    uint8_t irq_priority;
} dma_transfer_t;

typedef struct
{
    DMA_TypeDef *dma;
    uint8_t dma_ch_no; 
    void (*callback)(bw_status_t);
} dma_handle_t;

void hal_dma_init(dma_conf_t *conf, dma_handle_t *handle);
void hal_dma_start(dma_transfer_t *trnf, dma_handle_t *handle);
void hal_dma_isr(DMA_TypeDef *dma, uint8_t ch_no);
void hal_dma_denit(DMA_TypeDef *dma, uint8_t dma_ch_no);

#endif
