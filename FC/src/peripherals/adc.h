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
	
void adc_init();
uint16_t adc_capture(int chan);
void adc_scan(const int *chans, size_t count); // DMA mode, async
void adc_dual_scan(const int *chans1, size_t count1, const int *chans2, size_t count2); // DMA mode, async
void adc_set_sampletimes(const enum adc_sample_time *times);
bool adc_scan_finished();
void adc_disable_scan();
void *adc_dma_address();

#endif
