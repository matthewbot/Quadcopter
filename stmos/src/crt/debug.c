#include "debug.h"
#include <stdbool.h>
#include <stm32f10x.h>

void debug_init() {
	RCC->APB2ENR = RCC_APB2ENR_IOPCEN | 
	               RCC_APB2ENR_IOPAEN |
	               RCC_APB2ENR_USART1EN; // ensure IO port A, C and USART1 are enabled
	GPIOA->CRH = 0x444444B4; // make pin 9 alternate function output
	GPIOC->CRH = 0x44434444; // make pin 12 open drain output 
	debug_setled(false);
	
	USART1->CR1 = USART_CR1_UE; // set up USART1
	USART1->BRR = 72000000/DEBUG_BAUD;
	USART1->CR1 |= USART_CR1_TE;
}

void debug_setled(bool led) {
	if (led)
		GPIOC->BRR = (1 << 12);
	else
		GPIOC->BSRR = (1 << 12);
}

void debug_flashled(unsigned int cycles) {
	debug_setled(true);
	debug_delay(cycles);
	debug_setled(false);
}

void debug_print(const char *msg) {
	char ch;
	while ((ch = *msg++) != '\0') {
		while (!(USART1->SR & USART_SR_TXE)) { }
		USART1->DR = ch;
	}
	
	while (!(USART1->SR & USART_SR_TC)) { }
}

void debug_write(const uint8_t *data, size_t len) {
	while (len--) {
		while (!(USART1->SR & USART_SR_TXE)) { }
		USART1->DR = *data++;
	}
	
	while (!(USART1->SR & USART_SR_TC)) { }
}

void debug_printhex(uint32_t num) {
	static const char hexchars[16] = "0123456789ABCDEF";
	
	char buf[11] = "0x";
	int i;
	for (i=0; i<8;i++) {
		int val = num & 0x0f;
		num >>= 4;
		buf[9-i] = hexchars[val];
	}
	buf[10] = '\0';
	
	debug_print(buf);
}

void debug_delay(unsigned int cycles) {
	while (cycles--) { 
		asm volatile("nop;");
	}
}

