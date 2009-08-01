#include "peripherals/adc.h"
#include "stm32f10x.h"

#define ADC_CR2_EXTSEL_SWSTART (ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2)
#define ADC_CR1_DUALMOD_REGULAR (ADC_CR1_DUALMOD_1 | ADC_CR1_DUALMOD_2)

static uint32_t buildreg(const uint8_t **chanptr, size_t *count);
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

void adc_scan(const uint8_t *chans, size_t count, enum adc_trigger trigger) {
	adc_off();

	size_t countremaining = count;
	ADC1->SQR3 = buildreg(&chans, &countremaining);
	ADC1->SQR2 = buildreg(&chans, &countremaining);
	ADC1->SQR1 = buildreg(&chans, &countremaining) | ((count-1) << 20);
	
	ADC1->CR1 = ADC_CR1_SCAN; 
	if (trigger == ADC_TRIGGER_CONT) {
		ADC1->CR2 = ADC_CR2_TSVREFE | 
			        ADC_CR2_DMA | 
			        ADC_CR2_ADON | 
			        ADC_CR2_CONT | 
			        ADC_CR2_EXTTRIG | 
			        ADC_CR2_EXTSEL_SWSTART | 
			        ADC_CR2_SWSTART;
	} else {
		ADC1->CR2 = ADC_CR2_TSVREFE | 
			        ADC_CR2_DMA | 
			        ADC_CR2_ADON | 
			        ADC_CR2_EXTTRIG | 
			        ADC_CR2_SWSTART |
			        (trigger << 17);
	}
}

void adc_set_sampletime(int chan, enum adc_sample_time time) {
	adc_off();
	
	__IO uint32_t *SMPR_addr1, *SMPR_addr2;
	
	if (chan > 9) {
		SMPR_addr1 = &ADC1->SMPR1;
		SMPR_addr2 = &ADC2->SMPR1;
		chan -= 10;
	} else {
		SMPR_addr1 = &ADC1->SMPR2;
		SMPR_addr2 = &ADC2->SMPR2;
	}
	
	uint32_t reg = *SMPR_addr1;
	reg &= ~(0x3 << chan*3);
	reg |= time << chan*3;
	*SMPR_addr2 = *SMPR_addr1 = reg;
}

void *adc_dma_address() {
	return (void *)&ADC1->DR;
}

static uint32_t buildreg(const uint8_t **chanptr, size_t *count) { // I miss references
	uint32_t reg=0;
	int i;
	
	for (i=0; *count && i < 6; (*count)--, i++) { 
		reg |= (uint32_t)*(*chanptr)++ << i*5; // C++ invented references to deal with this
	}
	
	return reg;
}

static void adc_off() {
	ADC1->CR2 &= ~ADC_CR2_ADON;
	ADC2->CR2 &= ~ADC_CR2_ADON;
}
