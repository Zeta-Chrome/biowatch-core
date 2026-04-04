#include "hal/dma/dma.h"
#include "utils/assert.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"

#define DMA_CHANNEL_OFFSET(ch_no) (0x08 + 0x14 * (ch_no - 1))

static dma_handle_t *g_dma1_handles[6];
static dma_handle_t *g_dma2_handles[6];

static void dma_clock_init(DMA_TypeDef *dma)
{
    if (dma == DMA1)
    {
        reg_set_mask(&RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN_Msk);
    }
    else if (dma == DMA2)
    {
        reg_set_mask(&RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN_Msk);
    }
    reg_set_mask(&RCC->AHB1ENR, RCC_AHB1ENR_DMAMUX1EN_Msk);
}

void hal_dma_init(dma_conf_t *conf, dma_handle_t *handle)
{
    BW_ASSERT(conf->dma_ch_no >= 1 && conf->dma_ch_no <= 7, "Invalid DMA CH: %d (Expected 1-7)",
              conf->dma_ch_no);

    BW_ASSERT(conf->dmamux.num_req < 255, "Invalid DMAMUX requests: %d (Expected 1-254)",
              conf->dmamux.num_req);

    // Initialize clock
    dma_clock_init(conf->dma);

    DMA_Channel_TypeDef *dma_ch = (DMA_Channel_TypeDef *)((uint32_t)conf->dma +
                                                          DMA_CHANNEL_OFFSET(conf->dma_ch_no));
    // Configure CCR
    reg_set_field(&dma_ch->CCR, DMA_CCR_PL_Pos, 2, conf->priority);

    // Configure DMAMUX
    uint8_t dmamux_no = (conf->dma_ch_no - 1) + (conf->dma == DMA1 ? 0 : 7);
    DMAMUX_Channel_TypeDef *dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + 0x4UL * dmamux_no);

    reg_set_field(&dmamux->CCR, DMAMUX_CxCR_DMAREQ_ID_Pos, 6, conf->dmamux.dmareq_id);

    if (conf->dmamux.sync_en)
    {
        reg_set_field(&dmamux->CCR, DMAMUX_CxCR_SYNC_ID_Pos, 5, conf->dmamux.sync_id);
        reg_set_field(&dmamux->CCR, DMAMUX_CxCR_SPOL_Pos, 2, conf->dmamux.sync_pol);
        reg_set_field(&dmamux->CCR, DMAMUX_CxCR_NBREQ_Pos, 5, conf->dmamux.num_req + 1);
        reg_set_mask(&dmamux->CCR, DMAMUX_CxCR_SE_Msk);
    }
    else
    {
        reg_clear_mask(&dmamux->CCR, DMAMUX_CxCR_SE_Msk);
    }

    handle->dma = conf->dma;
    handle->dma_ch_no = conf->dma_ch_no;

    // Enable interrupts
    IRQn_Type irq;
    if (handle->dma == DMA1)
    {
        irq = DMA1_Channel1_IRQn + handle->dma_ch_no - 1;
    }
    else  // DMA2
    {
        irq = DMA2_Channel1_IRQn + handle->dma_ch_no - 1;
    }

    NVIC_EnableIRQ(irq);
}

void hal_dma_start(dma_transfer_t *trnf, dma_handle_t *handle)
{
    DMA_Channel_TypeDef *dma_ch = (DMA_Channel_TypeDef *)((uint32_t)handle->dma +
                                                          DMA_CHANNEL_OFFSET(handle->dma_ch_no));
    // Configure the count of data
    dma_ch->CNDTR = trnf->data_count;

    // Configure priority
    dma_ch->CPAR = trnf->per_addr;
    dma_ch->CMAR = trnf->mem_addr;

    // Configure CCR
    reg_set_field(&dma_ch->CCR, DMA_CCR_DIR_Pos, 1, trnf->mode == DMA_MODE_MEM_TO_PERI);
    reg_set_field(&dma_ch->CCR, DMA_CCR_PSIZE_Pos, 2, trnf->per_sz);
    reg_set_field(&dma_ch->CCR, DMA_CCR_MSIZE_Pos, 2, trnf->mem_sz);
    reg_set_field(&dma_ch->CCR, DMA_CCR_PINC_Pos, 1, trnf->per_incr_mode);
    reg_set_field(&dma_ch->CCR, DMA_CCR_MINC_Pos, 1, trnf->mem_incr_mode);
    reg_set_field(&dma_ch->CCR, DMA_CCR_CIRC_Pos, 1, trnf->circular);

    // Configure interrupts
    if (trnf->htc_trig)
    {
        reg_set_mask(&dma_ch->CCR, DMA_CCR_HTIE_Msk);
    }
    reg_set_mask(&dma_ch->CCR, DMA_CCR_TEIE_Msk);
    reg_set_mask(&dma_ch->CCR, DMA_CCR_TCIE_Msk);

    IRQn_Type irq;
    if (handle->dma == DMA1)
    {
        irq = DMA1_Channel1_IRQn + handle->dma_ch_no - 1;
    }
    else  // DMA2
    {
        irq = DMA2_Channel1_IRQn + handle->dma_ch_no - 1;
    }

    NVIC_SetPriority(irq, trnf->irq_priority);

    // Store handle then enable
    uint8_t idx = handle->dma_ch_no - 1;

    if (handle->dma == DMA1)
    {
        g_dma1_handles[idx] = handle;
    }
    else
    {
        g_dma2_handles[idx] = handle;
    }
    reg_set_mask(&dma_ch->CCR, DMA_CCR_EN_Msk);
}

void hal_dma_isr(DMA_TypeDef *dma, uint8_t ch_no)
{
    dma_handle_t *handle;
    if (dma == DMA1)
    {
        handle = g_dma1_handles[ch_no - 1];
    }
    else if (dma == DMA2)
    {
        handle = g_dma2_handles[ch_no - 1];
    }
    else
    {
        return;
    }
    DMA_Channel_TypeDef *dma_ch = (DMA_Channel_TypeDef *)((uint32_t)handle->dma +
                                                          DMA_CHANNEL_OFFSET(handle->dma_ch_no));

    if (reg_get_bit(&dma->ISR, (ch_no - 1) * 4 + 3))
    {
        reg_clear_bit(&dma->IFCR, (ch_no - 1) * 4 + 3);
        reg_clear_mask(&dma_ch->CCR, DMA_CCR_HTIE_Msk | DMA_CCR_TCIE_Msk | DMA_CCR_TEIE_Msk);
        handle->callback(STATUS_DMA_TERR);
        return;
    }

    if (reg_get_bit(&dma->ISR, (ch_no - 1) * 4 + 1))
    {
        reg_clear_bit(&dma->IFCR, (ch_no - 1) * 4 + 1);
        reg_clear_mask(&dma_ch->CCR, DMA_CCR_HTIE_Msk | DMA_CCR_TCIE_Msk | DMA_CCR_TEIE_Msk);
        handle->callback(STATUS_DMA_TC);
        return;
    }

    if (reg_get_bit(&dma->ISR, (ch_no - 1) * 4 + 2))
    {
        reg_clear_bit(&dma->IFCR, (ch_no - 1) * 4 + 2);
        reg_clear_mask(&dma_ch->CCR, DMA_CCR_HTIE_Msk);
        handle->callback(STATUS_DMA_HTC);
        return;
    }
}

void hal_dma_denit(DMA_TypeDef *dma, uint8_t dma_ch_no)
{
    BW_ASSERT(dma_ch_no >= 1 && dma_ch_no <= 7, "Invalid DMA CH: %d (Expected 1-7)", dma_ch_no);

    uint8_t dmamux_no = (dma_ch_no - 1) + (dma == DMA1 ? 0 : 7);
    DMAMUX_Channel_TypeDef *dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + 0x4UL * dmamux_no);
    reg_clear_mask(&dmamux->CCR, DMAMUX_CxCR_SE_Msk);

    DMA_Channel_TypeDef *dma_ch = (DMA_Channel_TypeDef *)((uint32_t)dma + DMA_CHANNEL_OFFSET(dma_ch_no));
    reg_clear_mask(&dma_ch->CCR, DMA_CCR_EN_Msk);

    IRQn_Type irq;
    if (dma == DMA1)
    {
        irq = DMA1_Channel1_IRQn + dma_ch_no - 1;
    }
    else  // DMA2
    {
        irq = DMA2_Channel1_IRQn + dma_ch_no - 1;
    }
    NVIC_DisableIRQ(irq);
}
