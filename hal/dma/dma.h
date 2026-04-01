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
    uint8_t nbytes;
    bool sync_en;
    uint8_t sync_id;
    dmamux_spol_t sync_pol;
} dmamux_conf_t;

typedef struct
{
    DMA_TypeDef *dma;
    uint8_t dma_ch_no;
    uint16_t data_count;
    dma_pl_t priority;
    dma_mode_t mode;
    bool htc_trig;
    void (*callback)(bw_status_t);
    uint32_t per_addr;
    uint32_t mem_addr;
    dma_sz_t mem_sz;
    dma_sz_t per_sz;
    bool per_incr_mode;
    bool mem_incr_mode;
    bool circ_mode;
    dmamux_conf_t dmamux;
} dma_conf_t;

void hal_dma_init(dma_conf_t *conf);
void hal_dma_denit(DMA_TypeDef *dma, uint8_t dma_ch_no);

#endif
