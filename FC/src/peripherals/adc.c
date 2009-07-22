#include "peripherals/adc.h"
#include "stm32f10x.h"
#include <stdio.h>

#define ADC_CR2_EXTSEL_SWSTART (ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2)
#define ADC_CR1_DUALMOD_REGULAR (ADC_CR1_DUALMOD_1 | ADC_CR1_DUALMOD_2)

static uint32_t buildreg(const int **chanptr, size_t *count);
static void adc_off();

void adc_init() {
	ADC1->CR2 = ADC_CR2_CAL | ADC_CR2_ADON;
	ADC2->CR2 = ADC_CR2_CAL | ADC_CR2_ADON;
	
	while (ADC1->CR2 & ADC_CR2_CAL) { }
	while (ADC2->CR2 & ADC_CR2_CAL) { }
}

uint16_t adc_capture(int chan) {
	adc_off();
	
	ADC1->SQR1 = 0; // set sequence length to 1 conversion
	ADC1->SQR3 = chan;
	ADC1->CR1 = ADC_CR1_SCAN; 
	ADC1->CR2 = ADC_CR2_TSVREFE | // enable internal temperature and vref sensors
                ADC_CR2_EXTTRIG | // enable "external" trigger
                ADC_CR2_EXTSEL_SWSTART | // set external trigger to software
                ADC_CR2_ADON | // stay on
                ADC_CR2_SWSTART; // start conversion
	
	while (!(ADC1->SR & ADC_SR_EOC)) { } // wait till end of conversion
	
	return ADC1->DR; 
}

void adc_scan(const int *chans, size_t count) {
	adc_off();

	size_t countremaining = count;
	ADC1->SQR3 = buildreg(&chans, &countremaining);
	ADC1->SQR2 = buildreg(&chans, &countremaining);
	ADC1->SQR1 = buildreg(&chans, &countremaining) | ((count-1) << 20);
	
	ADC1->CR1 = ADC_CR1_SCAN; 
	ADC1->CR2 = ADC_CR2_TSVREFE | 
	            ADC_CR2_DMA | 
	            ADC_CR2_ADON | 
	            ADC_CR2_CONT | 
	            ADC_CR2_EXTTRIG | 
	            ADC_CR2_EXTSEL_SWSTART | 
	            ADC_CR2_SWSTART; // start conversion
}

void adc_dual_scan(const int *chans1, size_t count1, const int *chans2, size_t count2) {
	adc_off();

	ADC2->SQR3 = buildreg(&chans2, &count2); // ADC2 first because ADC1 must be given start signal
	ADC2->SQR2 = buildreg(&chans2, &count2);
	ADC2->SQR1 = buildreg(&chans2, &count2) | (count2 << 20);
	ADC2->CR1 = ADC_CR1_SCAN;
	ADC2->CR2 = ADC_CR2_TSVREFE | ADC_CR2_DMA | ADC_CR2_ADON;
		
	ADC1->SQR3 = buildreg(&chans1, &count1);
	ADC1->SQR2 = buildreg(&chans1, &count1);
	ADC1->SQR1 = buildreg(&chans1, &count1) | (count1 << 20);
	ADC1->CR1 = ADC_CR1_SCAN | ADC_CR1_DUALMOD_REGULAR;
	ADC1->CR2 = ADC_CR2_TSVREFE | ADC_CR2_DMA | ADC_CR2_ADON | ADC_CR2_CONT; // start conversion
}

void adc_set_sampletimes(const enum adc_sample_time *times) {
	adc_off();

	int i;
	uint32_t reg;
	
	for (reg=0, i=0; i < 10; i++) {
		reg |= *times++ << i*3;
	}
	
	ADC1->SMPR2 = reg;
	ADC2->SMPR2 = reg;

	for (reg=0, i=0; i < 8; i++) {
		reg |= *times++ << i*3;
	}
	
	ADC1->SMPR1 = reg;
	ADC2->SMPR1 = reg;
}

bool adc_scan_finished() {
	return (ADC1->SR & ADC_SR_EOC) != 0;
}

void *adc_dma_address() {
	return (void *)&ADC1->DR;
}

static uint32_t buildreg(const int **chanptr, size_t *count) { // I miss references
	uint32_t reg=0;
	int i;
	
	for (i=0; *count && i < 6; (*count)--, i++) { 
		reg |= *(*chanptr)++ << i*5; // C++ invented references to deal with this
	}
	
	return reg;
}

static void adc_off() {
	ADC1->CR2 &= ~ADC_CR2_ADON;
	ADC2->CR2 &= ~ADC_CR2_ADON;
}
