#include "drivers/analog.h"
#include "peripherals/adc.h"
#include "peripherals/dma.h"
#include "peripherals/nvic.h"
#include <stdint.h>

#define DMA_ADC 1

#define READINGS_COUNT 6
static volatile uint16_t readings[READINGS_COUNT];

static const int chans[] = { 
	13, 1, 9, // Roll Pitch Yaw gyros
	12, 14, 15 // Accel X Y and Z
};

static const enum adc_sample_time sampletimes[] = {
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239,
	ADC_SAMPLE_TIME_239
};

void analog_init() {
	dma_configure(DMA_ADC, DMA_DIR_PERIPHERAL_TO_MEM, DMA_PRIORITY_HIGH, 16, 16, 
		DMA_OPTION_MEMORY_INCREMENT | DMA_OPTION_CIRCULAR);
	dma_start(DMA_ADC, readings, adc_dma_address(), READINGS_COUNT);
	
	adc_set_sampletimes(sampletimes);
	adc_scan(chans, READINGS_COUNT);
}

const volatile uint16_t *analog_get_raw() {
	return readings;
}
