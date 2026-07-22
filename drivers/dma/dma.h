#ifndef DRIVER_DMA_H
#define DRIVER_DMA_H

#include "lib/status.h"
#include "stm32wb55xx.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*dma_callback_t)(enum bw_status, void *);

enum dma_pl { DMA_PL_LOW, DMA_PL_MEDIUM, DMA_PL_HIGH, DMA_PL_VERY_HIGH };

enum dma_sz { DMA_SZ_8, DMA_SZ_16, DMA_SZ_32 };

enum dma_mode {
	DMA_MODE_PERI_TO_MEM,
	DMA_MODE_MEM_TO_PERI,
};

enum dmamux_spol { DMAMUX_SPOL_NONE, DMAMUX_SPOL_RISING, DMAMUX_SPOL_FALLING, DMAMUX_SPOL_BOTH };

struct dmamux_conf {
	uint8_t dmareq_id;
	bool sync_en;
	uint8_t sync_id;
	enum dmamux_spol sync_pol;
	uint8_t num_req;
};

struct dma_conf {
	DMA_TypeDef *dma;
	uint8_t ch_no;
	enum dma_pl priority;
	struct dmamux_conf dmamux;
	uint8_t irq_priority;
};

struct dma_transfer {
	uint16_t data_count;
	enum dma_mode mode;
	uint32_t per_addr;
	enum dma_sz per_sz;
	bool per_incr;
	uint32_t mem_addr;
	enum dma_sz mem_sz;
	bool mem_incr;
	bool circular;
	bool htc_trig;
};

struct dma_handle {
	DMA_TypeDef *dma;
	uint8_t ch_no;
	void *user_data;
	dma_callback_t callback;
};

void dma_init(struct dma_conf *conf, struct dma_handle *handle);
void dma_start(struct dma_handle *handle, struct dma_transfer *trnf);
void dma_denit(struct dma_handle *handle);

#endif
