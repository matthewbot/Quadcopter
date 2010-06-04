#include "ADC.h"
#include <stm32f10x.h>

#define ADC_CR2_JEXTSEL_JSWSTART (0x7 << 12)
#define ADC_CR2_EXTSEL_SWSTART (0x7 << 17)

using namespace stmos;

static ADC_TypeDef *const adcs[] = { 0, ADC1, ADC2 };

static void delay() { 
	int i; 
	for (i=0; i<100; i++) { asm("nop"); }
}

ADC::ADC(int num)
: num(num) {
	switch (num) {
		case 1:
			RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
			break;
			
		case 2:
			RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
			break;
	}
	
	ADC_TypeDef *adc = adcs[num];
	
	adc->CR1 = 0; // turn off ADC
	adc->CR2 = 0;
	delay(); // need some power-off time
	
	adc->CR2 |= ADC_CR2_CAL | ADC_CR2_ADON; // begin calibration
	while (adc->CR2 & ADC_CR2_CAL) { } // wait for calibration to complete
	
	adc->CR1 |= ADC_CR1_SCAN; // scan all selected channels
	adc->CR2 |= ADC_CR2_ALIGN | // set left alignment
	            ADC_CR2_JEXTSEL_JSWSTART | // select software start for injected channels
	            ADC_CR2_JEXTTRIG | // enable trigger for injected channels
	            ADC_CR2_EXTSEL_SWSTART | // select software start for regular channels
	            ADC_CR2_EXTTRIG; // enable trigger for regular channels
	            
	if (num == 1) 
		adc->CR2 |= ADC_CR2_TSVREFE;
}

void ADC::setSampleChannel(Channel chan) {
	ADC_TypeDef *adc = adcs[num];
	
	adc->JSQR = chan << 15; // set injected sequence length to 1
}

void ADC::setSampleChannels(const Channel *chan, size_t chancount) {
	ADC_TypeDef *adc = adcs[num];
	
	int n = 4 - chancount;
	uint32_t jsqr = (chancount-1) << 20;
	while (chancount--) {
		jsqr |= *chan++ << (n++ * 5);
	}
	
	adc->JSQR = jsqr;
}

static void injectedsample(ADC_TypeDef *adc);

ADC::Sample ADC::sample() {
	ADC_TypeDef *adc = adcs[num];
	
	injectedsample(adc);
	
	return adc->JDR1 << 1;
}

void ADC::sampleMultiple(ADC::Sample *samples, size_t samplecount) {
	ADC_TypeDef *adc = adcs[num];

	injectedsample(adc);
	
	samples += samplecount;
	switch (samplecount) {
		case 4:
			*--samples = adc->JDR4 << 1;
		case 3:
			*--samples = adc->JDR3 << 1;
		case 2:
			*--samples = adc->JDR2 << 1;
		case 1:
			*--samples = adc->JDR1 << 1;
	}	
}

static void injectedsample(ADC_TypeDef *adc) {
	adc->CR2 |= ADC_CR2_JSWSTART;
	
	while (!(adc->SR & ADC_SR_JEOC)) { }
	adc->SR &= ~ADC_SR_JEOC;
}

void ADC::setSampleTime(ADC::Channel chan, ADC::SampleTime time) {
	ADC_TypeDef *adc = adcs[num];
	__IO uint32_t *SMPR;
	
	if (chan > 9) {
		SMPR = &adc->SMPR1;
		chan -= 10;
	} else {
		SMPR = &adc->SMPR2;
	}
	
	uint32_t reg = *SMPR;
	reg &= ~(0x03 << (chan*3));
	reg |= (time << (chan*3));
	*SMPR = reg;
}

static uint32_t buildreg(const uint8_t *&chanptr, size_t &count);

void ADC::setScanChannels(const Channel *chans, size_t chancount) {
	ADC_TypeDef *adc = adcs[num];

	size_t countremaining = chancount;
	adc->SQR3 = buildreg(chans, countremaining);
	adc->SQR2 = buildreg(chans, countremaining);
	adc->SQR1 = buildreg(chans, countremaining) | ((chancount-1) << 20);
}

static uint32_t buildreg(const uint8_t *&chanptr, size_t &count) {
	uint32_t reg=0;
	int i;
	
	for (i=0; count && i < 6; count--, i++) { 
		reg |= ((uint32_t)*chanptr++) << i*5; 
	}
	
	return reg;
}

void ADC::startScan() {
	ADC_TypeDef *adc = adcs[num];
	
	adc->CR2 |= ADC_CR2_SWSTART | ADC_CR2_CONT | ADC_CR2_DMA;
}

void ADC::stopScan() {
	ADC_TypeDef *adc = adcs[num];
	
	adc->CR2 &= ~(ADC_CR2_CONT | ADC_CR2_DMA);
}

void ADC::singleScan() {
	ADC_TypeDef *adc = adcs[num];
	
	adc->CR2 |= ADC_CR2_SWSTART | ADC_CR2_DMA;
}

volatile void *ADC::getScanDMAAddress() const {
	ADC_TypeDef *adc = adcs[num];
	return &adc->DR;
}

int ADC::getScanDMAChannel() const {
	return 1;
}

IOPin::PortPin ADC::getChannelPortPin(ADC::Channel chan) {
	IOPin::PortPin ret;
	if (chan <= 7) {
		ret.port = IOPin::PORT_A;
		ret.pin = chan;
	} else if (chan <= 9) {
		ret.port = IOPin::PORT_B;
		ret.pin = chan - 8;
	} else {
		ret.port = IOPin::PORT_C;
		ret.pin = chan - 10;
	}
	
	return ret;
}

