#include "drivers/imu.h"
#include "peripherals/adc.h"
#include "peripherals/dma.h"
#include "peripherals/nvic.h"
#include <stdint.h>

#define DMA_ADC 1

static uint16_t imu_buf[6];

static const int imu_chans[] = { 
	13, 1, 9, // Roll Pitch Yaw gyros
	12, 14, 15 // Accel X Y and Z
};

static const enum adc_sample_time imu_sampletimes[] = {
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

void imu_init() {
	dma_configure(DMA_ADC, DMA_DIR_PERIPHERAL_TO_MEM, DMA_PRIORITY_HIGH, 16, 16, 
		DMA_OPTION_MEMORY_INCREMENT | DMA_OPTION_CIRCULAR);
	dma_start(DMA_ADC, imu_buf, adc_dma_address(), sizeof(imu_buf) / sizeof(imu_buf[0]));
	
	adc_set_sampletimes(imu_sampletimes);
	adc_scan(imu_chans, sizeof(imu_chans) / sizeof(imu_chans[0]));
}

const uint16_t *imu_read_raw() {
	return imu_buf;
}
