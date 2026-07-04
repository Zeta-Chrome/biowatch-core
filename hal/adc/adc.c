#include "adc.h"
#include "assert.h"
#include "gpio/gpio.h"
#include "gpio/gpio_types.h"
#include "stm32wb55xx.h"
#include "systick/systick.h"
#include "utils.h"
#include "utils/status.h"

static adc_handle_t *g_adc_h;

static void adc_clock_init()
{
    MODIFY_FIELD(ADC1_COMMON->CCR, ADC_CCR_CKMODE_Msk, ADC_CCR_CKMODE_Pos, 0x0);
    MODIFY_FIELD(RCC->CCIPR, RCC_CCIPR_ADCSEL_Msk, RCC_CCIPR_ADCSEL_Pos, 0x3);
    SET_FIELD(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN_Msk);
}

static void adc_hw_conf(adc_conf_t *conf)
{
    adc_clock_init();

    // Calibrate ADC
    CLEAR_FIELD(ADC1->CR, ADC_CR_ADEN_Msk);
    CLEAR_FIELD(ADC1->CR, ADC_CR_DEEPPWD_Msk); // ADEN = 0 for clearing this
    SET_FIELD(ADC1->CR, ADC_CR_ADVREGEN_Msk);
    hal_systick_delay_us(20); // wait for minimum 20us for regulator startup time

    // Set single ended adc or differential adc
    MODIFY_BIT(ADC1->CR, ADC_CR_ADCALDIF_Pos, conf->inp);

    // Enter into calibration mode
    SET_FIELD(ADC1->CR, ADC_CR_ADCAL_Msk);
    while (ADC1->CR & ADC_CR_ADCAL_Msk);

    // Enable ADC
    CLEAR_FIELD(ADC1->ISR, ADC_ISR_ADRDY_Msk);
    SET_FIELD(ADC1->CR, ADC_CR_ADEN_Msk);
    while (!(ADC1->ISR & ADC_ISR_ADRDY_Msk));
    SET_FIELD(ADC1->ISR, ADC_ISR_ADRDY_Msk);

    if (conf->inlen == 0)
    {
        return;
    }

    BW_ASSERT(conf->inlen <= 4, "Invalid No of ADC channels %d (Expected 0-4)");

    // Wait till no more conversions
    while (ADC1->CR & (ADC_CR_ADSTART_Msk | ADC_CR_JADSTART_Msk));

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
            MODIFY_FIELD_W(ADC1->SMPR1, 3, 3 * in, conf->smp[i]);
        }
        else
        {
            MODIFY_FIELD_W(ADC1->SMPR2, 3, 3 * (in - 10), conf->smp[i]);
        }

        // Select adc input type (single ended or differential)
        MODIFY_BIT(ADC1->DIFSEL, in, conf->inp);
    }

    // Set single conversion mode (CONT = 0)
    CLEAR_FIELD(ADC1->CFGR, ADC_CFGR_CONT_Msk);
}

void hal_adc_init(adc_conf_t *conf)
{
    adc_hw_conf(conf);

    NVIC_SetPriority(ADC1_IRQn, conf->irq_priority);
    NVIC_EnableIRQ(ADC1_IRQn);
}

void hal_adc_isr()
{
    if (READ_BIT(ADC1->ISR, ADC_ISR_EOC_Pos) == 1)
    {
        uint16_t raw = ADC1->DR;
        *(g_adc_h->buf + g_adc_h->buf_count++) = raw;
    }

    if (READ_BIT(ADC1->ISR, ADC_ISR_EOS_Pos) == 1)
    {
        // Clear the EOC & EOS and disable interrupts (rc_w1)
        WRITE_FIELD(ADC1->ISR, ADC_ISR_EOC_Msk | ADC_ISR_EOS_Msk);
        CLEAR_FIELD(ADC1->IER, ADC_IER_EOCIE_Msk | ADC_IER_EOSIE_Msk);
        g_adc_h->callback(STATUS_OK, g_adc_h->user_data);
        return;
    }
}

static void adc_prep_conversion(adc_handle_t *handle)
{
    handle->buf_count = 0;
    g_adc_h = handle;

    if (handle->inseqlen == 0)
    {
        return;
    }

    BW_ASSERT(handle->inseqlen <= 4, "Invalid ADC Sequence Length %d (Expected 0-4)");

    // Wait till no more conversions
    while (ADC1->CR & (ADC_CR_ADSTART_Msk | ADC_CR_JADSTART_Msk));

    // Configure channel sequence
    MODIFY_FIELD(ADC1->SQR1, ADC_SQR1_L_Msk, ADC_SQR1_L_Pos, handle->inseqlen - 1);
    for (uint8_t i = 0; i < handle->inseqlen; i++)
    {
        uint8_t in = handle->inseq[i];
        MODIFY_FIELD(ADC1->SQR1, ADC_SQR1_SQ1_Msk, ADC_SQR1_SQ1_Pos + 6 * i, in);
    }

    // configure OVRMOD
    SET_FIELD(ADC1->CFGR, ADC_CFGR_OVRMOD_Msk);
}

void hal_adc_convert(adc_handle_t *handle)
{
    // Clear old configurations
    CLEAR_FIELD(ADC1->CFGR, ADC_CFGR_DMAEN_Msk | ADC_CFGR_DMACFG_Msk);

    // Prepare conversion
    adc_prep_conversion(handle);

    // Enable interrupts
    SET_FIELD(ADC1->IER, ADC_IER_EOCIE_Msk | ADC_IER_EOSIE_Msk);

    // Start conversion
    SET_FIELD(ADC1->CR, ADC_CR_ADSTART_Msk);
}

void hal_adc_deinit()
{
    // Force stop all conversions
    CLEAR_FIELD(ADC1->CR, ADC_CR_ADSTART_Msk | ADC_CR_JADSTART_Msk);

    // Wait till no more conversions
    while (ADC1->CR & (ADC_CR_ADSTART_Msk | ADC_CR_JADSTART_Msk));

    // set addis
    SET_FIELD(ADC1->CR, ADC_CR_ADDIS_Msk);

    // wait till complete disable
    while (ADC1->CR & ADC_CR_ADEN_Pos);

    // Save more power
    CLEAR_FIELD(ADC1->CR, ADC_CR_ADVREGEN_Msk);

    // Disable irq
    NVIC_DisableIRQ(ADC1_IRQn);
}
