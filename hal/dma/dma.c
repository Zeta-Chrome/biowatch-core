#include "dma.h"
#include "stm32wb55xx.h"
#include "utils.h"
#include "utils/assert.h"

#define DMA_CHANNEL_OFFSET(ch_no) (0x08 + 0x14 * (ch_no - 1))

static dma_handle_t *g_dma1_handles[6];
static dma_handle_t *g_dma2_handles[6];

static void dma_clock_init(DMA_TypeDef *dma)
{
    if (dma == DMA1)
    {
        SET_FIELD(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN_Msk);
    }
    else if (dma == DMA2)
    {
        SET_FIELD(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN_Msk);
    }
    SET_FIELD(RCC->AHB1ENR, RCC_AHB1ENR_DMAMUX1EN_Msk);
}

void hal_dma_init(dma_conf_t *conf, dma_handle_t *handle)
{
    BW_ASSERT(conf->ch_no >= 1 && conf->ch_no <= 7, "Invalid DMA CH: %d (Expected 1-7)", conf->ch_no);

    BW_ASSERT(conf->dmamux.num_req < 255,
              "Invalid DMAMUX requests: %d (Expected 1-254)",
              conf->dmamux.num_req);

    // Initialize clock
    dma_clock_init(conf->dma);

    DMA_Channel_TypeDef *dma_ch =
        (DMA_Channel_TypeDef *)((uint32_t)conf->dma + DMA_CHANNEL_OFFSET(conf->ch_no));

    // Configure CCR
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_PL_Msk, DMA_CCR_PL_Pos, conf->priority);

    // Configure DMAMUX
    uint8_t dmamux_no = (conf->ch_no - 1) + (conf->dma == DMA1 ? 0 : 7);
    DMAMUX_Channel_TypeDef *dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + 0x4UL * dmamux_no);

    MODIFY_FIELD(dmamux->CCR, DMAMUX_CxCR_DMAREQ_ID_Msk, DMAMUX_CxCR_DMAREQ_ID_Pos, conf->dmamux.dmareq_id);

    if (conf->dmamux.sync_en)
    {
        MODIFY_FIELD(dmamux->CCR, DMAMUX_CxCR_SYNC_ID_Msk, DMAMUX_CxCR_SYNC_ID_Pos, conf->dmamux.sync_id);
        MODIFY_FIELD(dmamux->CCR, DMAMUX_CxCR_SPOL_Msk, DMAMUX_CxCR_SPOL_Pos, conf->dmamux.sync_pol);
        MODIFY_FIELD(dmamux->CCR, DMAMUX_CxCR_NBREQ_Msk, DMAMUX_CxCR_NBREQ_Pos, conf->dmamux.num_req + 1);
        SET_FIELD(dmamux->CCR, DMAMUX_CxCR_SE_Msk);
    }
    else
    {
        CLEAR_FIELD(dmamux->CCR, DMAMUX_CxCR_SE_Msk);
    }

    handle->dma = conf->dma;
    handle->ch_no = conf->ch_no;

    IRQn_Type irq;
    if (handle->dma == DMA1)
    {
        irq = DMA1_Channel1_IRQn + handle->ch_no - 1;
    }
    else // DMA2
    {
        irq = DMA2_Channel1_IRQn + handle->ch_no - 1;
    }

    NVIC_EnableIRQ(irq);
    NVIC_SetPriority(irq, conf->irq_priority);
}

void hal_dma_start(dma_handle_t *handle, dma_transfer_t *trnf)
{
    DMA_Channel_TypeDef *dma_ch =
        (DMA_Channel_TypeDef *)((uint32_t)handle->dma + DMA_CHANNEL_OFFSET(handle->ch_no));

    // Configure priority
    dma_ch->CPAR = trnf->per_addr;
    dma_ch->CMAR = trnf->mem_addr;

    // Configure the count of data
    dma_ch->CNDTR = trnf->data_count;

    // Configure CCR
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_DIR_Msk, DMA_CCR_DIR_Pos, trnf->mode == DMA_MODE_MEM_TO_PERI);
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_PSIZE_Msk, DMA_CCR_PSIZE_Pos, trnf->per_sz);
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_MSIZE_Msk, DMA_CCR_MSIZE_Pos, trnf->mem_sz);
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_PINC_Msk, DMA_CCR_PINC_Pos, trnf->per_incr);
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_MINC_Msk, DMA_CCR_MINC_Pos, trnf->mem_incr);
    MODIFY_FIELD(dma_ch->CCR, DMA_CCR_CIRC_Msk, DMA_CCR_CIRC_Pos, trnf->circular);

    // Configure interrupts
    if (trnf->htc_trig)
    {
        SET_FIELD(dma_ch->CCR, DMA_CCR_HTIE_Msk);
    }
    SET_FIELD(dma_ch->CCR, DMA_CCR_TEIE_Msk | DMA_CCR_TCIE_Msk);

    // Store handle then enable
    uint8_t idx = handle->ch_no - 1;

    if (handle->dma == DMA1)
    {
        g_dma1_handles[idx] = handle;
    }
    else
    {
        g_dma2_handles[idx] = handle;
    }
    SET_FIELD(dma_ch->CCR, DMA_CCR_EN_Msk);
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
    DMA_Channel_TypeDef *dma_ch =
        (DMA_Channel_TypeDef *)((uint32_t)handle->dma + DMA_CHANNEL_OFFSET(handle->ch_no));

    if (READ_BIT(dma->ISR, (ch_no - 1) * 4 + 3)) // TERR
    {
        SET_FIELD(dma->IFCR, 0xF << ((ch_no - 1) * 4));
        CLEAR_FIELD(dma_ch->CCR, DMA_CCR_HTIE_Msk | DMA_CCR_TCIE_Msk | DMA_CCR_TEIE_Msk);
        if (!(dma_ch->CCR & DMA_CCR_CIRC_Msk))
        {
            CLEAR_FIELD(dma_ch->CCR, DMA_CCR_EN_Msk);
        }
        if (handle->callback)
        {
            handle->callback(STATUS_DMA_TERR, handle->user_data);
        }
        return;
    }

    if (READ_BIT(dma->ISR, (ch_no - 1) * 4 + 1)) // TC
    {
        SET_FIELD(dma->IFCR, 0xF << ((ch_no - 1) * 4));
        CLEAR_FIELD(dma_ch->CCR, DMA_CCR_HTIE_Msk | DMA_CCR_TCIE_Msk | DMA_CCR_TEIE_Msk | DMA_CCR_EN_Msk);
        if (handle->callback)
        {
            handle->callback(STATUS_DMA_TC, handle->user_data);
        }
        return;
    }

    if (READ_BIT(dma->ISR, (ch_no - 1) * 4 + 2)) // HTC
    {
        SET_BIT(dma->IFCR, (ch_no - 1) * 4 + 2);
        CLEAR_FIELD(dma_ch->CCR, DMA_CCR_HTIE_Msk);
        if (handle->callback)
        {
            handle->callback(STATUS_DMA_HTC, handle->user_data);
        }
        return;
    }
}

void hal_dma_denit(dma_handle_t *handle)
{
    BW_ASSERT(handle->ch_no >= 1 && handle->ch_no <= 7, "Invalid DMA CH: %d (Expected 1-7)", handle->ch_no);

    uint8_t dmamux_no = (handle->ch_no - 1) + (handle->dma == DMA1 ? 0 : 7);
    DMAMUX_Channel_TypeDef *dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + 0x4UL * dmamux_no);
    CLEAR_FIELD(dmamux->CCR, DMAMUX_CxCR_SE_Msk);

    DMA_Channel_TypeDef *dma_ch =
        (DMA_Channel_TypeDef *)((uint32_t)handle->dma + DMA_CHANNEL_OFFSET(handle->ch_no));
    CLEAR_FIELD(dma_ch->CCR, DMA_CCR_EN_Msk);

    IRQn_Type irq;
    if (handle->dma == DMA1)
    {
        irq = DMA1_Channel1_IRQn + handle->ch_no - 1;
    }
    else // DMA2
    {
        irq = DMA2_Channel1_IRQn + handle->ch_no - 1;
    }
    NVIC_DisableIRQ(irq);
}
