#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H

#include "drivers/gpio/gpio_types.h"
#include "lib/status.h"

#define MAX_ADC_IN 4

typedef void (*adc_callback_t)(enum bw_status, void *);

enum adc_inp { ADC_INP_SINGLE, ADC_INP_DIFFERENTIAL };

enum adc_smp {
	ADC_SMP_2_5_CLK,
	ADC_SMP_6_5_CLK,
	ADC_SMP_12_5_CLK,
	ADC_SMP_24_5_CLK,
	ADC_SMP_47_5_CLK,
	ADC_SMP_92_5_CLK,
	ADC_SMP_640_5_CLK,
};

enum adc_ch {
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
};

struct adc_conf {
	enum adc_inp inp;
	struct gpio gpios[MAX_ADC_IN];
	enum adc_smp smp[MAX_ADC_IN];
	enum adc_ch in[MAX_ADC_IN];
	uint8_t inlen; // 0 - 4
	uint8_t irq_priority; // 0 - 15
};

struct adc_handle {
	uint16_t *buf;
	enum adc_ch inseq[MAX_ADC_IN];
	uint8_t inseqlen; // 0 - 4
	uint8_t buf_count;
	void *user_data;
	adc_callback_t callback;
};

void adc_init(struct adc_conf *conf);
void adc_convert(struct adc_handle *handle);
void adc_deinit();

#endif
