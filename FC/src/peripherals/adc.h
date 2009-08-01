#ifndef FC_PERIPHERALS_ADC_H
#define FC_PERIPHERALS_ADC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum adc_sample_time {
	ADC_SAMPLE_TIME_1,
	ADC_SAMPLE_TIME_7,
	ADC_SAMPLE_TIME_13,
	ADC_SAMPLE_TIME_28,	
	ADC_SAMPLE_TIME_41,
	ADC_SAMPLE_TIME_55,
	ADC_SAMPLE_TIME_71,
	ADC_SAMPLE_TIME_239
};	
	
enum adc_trigger {
	ADC_TRIGGER_TIM1_CC1,
	ADC_TRIGGER_TIM1_CC2,
	ADC_TRIGGER_TIM1_CC3,
	ADC_TRIGGER_TIM2_CC2,
	ADC_TRIGGER_TIM3_TRGO,
	ADC_TRIGGER_TIM4_CC4,
	ADC_TRIGGER_CONT, // instead of EXTI line 11
	ADC_TRIGGER_NOW
};
	
void adc_init();
uint16_t adc_capture(int chan);
void adc_scan(const uint8_t *chans, size_t count, enum adc_trigger trigger); // DMA mode
void adc_set_sampletime(int chan, enum adc_sample_time time);
void adc_disable_scan();
void *adc_dma_address();

#endif
