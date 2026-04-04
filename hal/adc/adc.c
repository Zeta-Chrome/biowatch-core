#include "hal/adc/adc.h"
#include "assert.h"
#include "hal/gpio/gpio.h"
#include "hal/gpio/gpio_types.h"
#include "hal/reg.h"
#include "stm32wb55xx.h"
#include "hal/systick/systick.h"

#define ADC_DMAREQ_ID 5

static adc_handle_t *adc_h;
static void adc_clock_init()
{
    reg_set_field(&ADC1_COMMON->CCR, ADC_CCR_CKMODE_Pos, 0, 0x0);
    reg_set_field(&RCC->CCIPR, RCC_CCIPR_ADCSEL_Pos, 2, 0x3);
    reg_set_mask(&RCC->AHB2ENR, RCC_AHB2ENR_ADCEN_Msk);
}

void hal_adc_init(adc_conf_t *conf)
{
    adc_clock_init();

    // Calibrate ADC
    reg_clear_mask(&ADC1->CR, ADC_CR_DEEPPWD_Msk);
    reg_set_mask(&ADC1->CR, ADC_CR_ADVREGEN_Msk);
    hal_systick_delay_us(20);  // wait for minimum 20us for regulator startup time

    // Disable ADC before calibration
    reg_clear_mask(&ADC1->CR, ADC_CR_ADEN_Msk);

    reg_set_field(&ADC1->CR, ADC_CR_ADCALDIF_Pos, 1, conf->inp);

    // Enter into calibration mode
    reg_set_mask(&ADC1->CR, ADC_CR_ADCAL_Msk);
    while (reg_get_bit(&ADC1->CR, ADC_CR_ADCAL_Pos) != 0);

    // Enable ADC
    reg_clear_mask(&ADC1->ISR, ADC_ISR_ADRDY_Msk);
    reg_set_mask(&ADC1->CR, ADC_CR_ADEN_Msk);
    while (reg_get_bit(&ADC1->ISR, ADC_ISR_ADRDY_Pos) != 1);
    reg_set_mask(&ADC1->ISR, ADC_ISR_ADRDY_Msk);

    if (conf->inlen == 0)
    {
        return;
    }

    BW_ASSERT(conf->inlen <= 4, "Invalid No of ADC channels %d (Expected 0-4)");

    // Wait till no more conversions
    while (reg_get_bit(&ADC1->CR, ADC_CR_ADSTART_Pos) != 0 &&
           reg_get_bit(&ADC1->CR, ADC_CR_JADSTART_Pos) != 0);

    // Configure channels
    gpio_conf_t gpio_conf = gpio_conf_analog(conf->gpios[0]);
    for (uint8_t i = 0; i < conf->inlen; i++)
    {
        gpio_conf.gpio = conf->gpios[i];
        hal_gpio_init(&gpio_conf);

        uint8_t in = conf->in[i];

        // Configure sampling time
        if (in < 10)
        {
            reg_set_field(&ADC1->SMPR1, 3 * in, 3, conf->smp[i]);
        }
        else
        {
            reg_set_field(&ADC1->SMPR2, 3 * (in - 10), 3, conf->smp[i]);
        }
        // Select adc input type (single ended or differential)
        reg_set_bit(&ADC1->DIFSEL, conf->inp);

        // Set single conversion mode (CONT = 0)
        reg_clear_mask(&ADC1->CFGR, ADC_CFGR_CONT);
    }
}

void hal_adc_convert(adc_handle_t *handle)
{
    handle->buf_count = 0;
    adc_h = handle;

    if (handle->inseqlen == 0)
    {
        return;
    }

    BW_ASSERT(handle->inseqlen <= 4, "Invalid ADC Sequence Length %d (Expected 0-4)");

    // Wait till no more conversions
    while (reg_get_bit(&ADC1->CR, ADC_CR_ADSTART_Pos) != 0 &&
           reg_get_bit(&ADC1->CR, ADC_CR_JADSTART_Pos) != 0);

    // Configure channel sequence
    reg_set_field(&ADC1->SQR1, ADC_SQR1_L_Pos, 3, (uint32_t)(handle->inseqlen - 1));
    for (uint8_t i = 0; i < handle->inseqlen; i++)
    {
        uint8_t in = handle->inseq[i];
        reg_set_field(&ADC1->SQR1, ADC_SQR1_SQ1_Pos + 6 * i, 5, in);
    }

    // configure OVRMOD
    reg_set_mask(&ADC1->CFGR, ADC_CFGR_OVRMOD_Msk);

    // Enable interrupts
    NVIC_SetPriority(ADC1_IRQn, handle->irq_priority);
    NVIC_EnableIRQ(ADC1_IRQn);
    reg_set_mask(&ADC1->IER, ADC_IER_EOCIE_Msk | ADC_IER_EOSIE_Msk);

    // Start conversion
    reg_set_mask(&ADC1->CR, ADC_CR_ADSTART_Msk);
}

void hal_adc_isr()
{
    if (reg_get_bit(&ADC1->ISR, ADC_ISR_EOC_Pos) == 1)
    {
        *(adc_h->buf + adc_h->buf_count++) = ADC1->DR;
    }

    if (reg_get_bit(&ADC1->ISR, ADC_ISR_EOS_Pos) == 1)
    {
        ADC1->ISR = ADC_ISR_EOS_Msk;
        reg_clear_mask(&ADC1->IER, ADC_IER_EOCIE_Msk);
        reg_clear_mask(&ADC1->IER, ADC_IER_EOSIE_Msk);
        adc_h->callback(STATUS_OK);
    }
}

void hal_adc_deinit(ADC_TypeDef *adc)
{
    // Force stop all conversions
    reg_clear_mask(&adc->CR, ADC_CR_ADSTART_Msk);
    reg_clear_mask(&adc->CR, ADC_CR_JADSTART_Msk);

    // Wait till no more conversions
    while (reg_get_bit(&adc->CR, ADC_CR_ADSTART_Pos) != 0 &&
           reg_get_bit(&adc->CR, ADC_CR_JADSTART_Pos) != 0);

    // set addis
    reg_set_mask(&adc->CR, ADC_CR_ADDIS_Msk);

    // wait till complete disable
    while (reg_get_bit(&adc->CR, ADC_CR_ADEN_Pos) != 0);

    // Save more power
    reg_clear_mask(&ADC1->CR, ADC_CR_ADVREGEN_Msk);

    // Disable irq
    NVIC_DisableIRQ(ADC1_IRQn);
}
