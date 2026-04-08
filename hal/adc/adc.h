#ifndef HAL_ADC_H
#define HAL_ADC_H

#include "utils/status.h"
#include "hal/gpio/gpio_types.h"

#define MAX_ADC_IN 4

typedef void (*adc_callback_t)(bw_status_t, void *);

typedef enum
{
    ADC_INP_SINGLE,
    ADC_INP_DIFFERENTIAL
} adc_inp_t;

typedef enum
{
    ADC_SMP_2_5_CLK,
    ADC_SMP_6_5_CLK,
    ADC_SMP_12_5_CLK,
    ADC_SMP_24_5_CLK,
    ADC_SMP_47_5_CLK,
    ADC_SMP_92_5_CLK,
    ADC_SMP_640_5_CLK,
} adc_smp_t;

typedef enum
{
    ADC_CH_PC0 = 1,
    ADC_CH_PC1 = 2,
    ADC_CH_PC2 = 3,
    ADC_CH_PC3 = 4,
    ADC_CH_PA0 = 5,
    ADC_CH_PA1 = 6,
    ADC_CH_PA2 = 7,
    ADC_CH_PA3 = 8,
    ADC_CH_PA4 = 9,
    ADC_CH_PA5 = 10,
    ADC_CH_PA6 = 11,
    ADC_CH_PA7 = 12,
    ADC_CH_PC4 = 13,
    ADC_CH_PC5 = 14,
    ADC_CH_PA8 = 15,
    ADC_CH_PA9 = 16,
} adc_ch_t;

typedef struct
{
    adc_inp_t inp;
    gpio_t gpios[MAX_ADC_IN];
    adc_smp_t smp[MAX_ADC_IN];
    adc_ch_t in[MAX_ADC_IN];
    uint8_t inlen;         // 0 - 4
    uint8_t irq_priority;  // 0 - 15
} adc_conf_t;

typedef struct
{
    uint16_t *buf;
    adc_ch_t inseq[MAX_ADC_IN];
    uint8_t inseqlen;  // 0 - 4
    uint8_t buf_count;
    void *user_data;
    adc_callback_t callback;
} adc_handle_t;

void hal_adc_init(adc_conf_t *conf);
void hal_adc_init_dma(adc_conf_t *conf);
void hal_adc_convert(adc_handle_t *handle);
void hal_adc_convert_dma(adc_handle_t *handle);
void hal_adc_deinit();
void hal_adc_deinit_dma();

#endif
