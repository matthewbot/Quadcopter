#include "peripherals/rcc.h"
#include "stm32f10x.h"

void rcc_init() {
	RCC->CR |= RCC_CR_HSEON; // enable HSE oscillator
	while (!(RCC->CR & RCC_CR_HSERDY)) { } // wait for it to come up
	
	FLASH->ACR |= FLASH_ACR_LATENCY_2; // set two flash wait states
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // set the APB Low-speed prescaler to divide HCLK by 2 (72 / 2 = 36, which is the max)
	
	RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9; // set PLL to multiply HSE by 9 (8Mhz * 9 = 72Mhz)
	RCC->CR |= RCC_CR_PLLON; // enable PLL
	while (!(RCC->CR & RCC_CR_PLLRDY)) { } // wait for PLL to come up
	
	RCC->CFGR |= RCC_CFGR_SW_PLL; // set PLL as system clock source
	while (!(RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) { } // wait until the switch is complete
	
	RCC->AHBENR = RCC_AHBENR_DMA1EN; // enable DMA clock
	RCC->APB1ENR = RCC_APB1ENR_I2C1EN | // enable I2C1
	               RCC_APB1ENR_USART3EN | // enable USART3
	               RCC_APB1ENR_TIM2EN | // enable timer 2
	               RCC_APB1ENR_TIM3EN | // enable timer 3
	               RCC_APB1ENR_TIM4EN; // enable timer 4
	RCC->APB2ENR = RCC_APB2ENR_USART1EN | // enable USART1 
	               RCC_APB2ENR_ADC1EN |  // enable ADC1
	               RCC_APB2ENR_ADC2EN | // enable ADC2
	               RCC_APB2ENR_TIM1EN | // enable TIM1
	               RCC_APB2ENR_IOPAEN | // enable port A
	               RCC_APB2ENR_IOPBEN | // enable port B
	               RCC_APB2ENR_IOPCEN | // enable port C
	               RCC_APB2ENR_IOPDEN | // enable port D
	               RCC_APB2ENR_AFIOEN; // enable alternate function I/O clock
}
