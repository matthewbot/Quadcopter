#include "peripherals/usart.h"
#include "stm32f10x.h"
#include <math.h>

static USART_TypeDef *const usarts[] = { 0, USART1, USART2, USART3 };

void usart_setup(int num, unsigned int baud, bool dma) {
	USART_TypeDef *usart = usarts[num];
	
	unsigned int baseclk = (num == 1) ? 4500000 : 2250000;
	unsigned int mantissa = baseclk / baud;
	unsigned int fraction = (unsigned int)(fmodf(baseclk, (float)baud) * 16);
	
	usart->CR1 = 0;
	usart->BRR = (mantissa << 4) | fraction;
	usart->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	if (dma)
		usart->CR3 = USART_CR3_DMAT | USART_CR3_DMAR;
}

void usart_disable(int num) {
	usarts[num]->CR1 = 0;
}

void usart_send(int num, const uint8_t *buf, size_t size) {
	USART_TypeDef *usart = usarts[num];
	
	while (size--) {
		usart->DR = *buf++;
		while (!(usart->SR & USART_SR_TXE)) { }
	}
}

void usart_receive(int num, uint8_t *buf, size_t size) {
	USART_TypeDef *usart = usarts[num];
	
	while (size--) {
		while (!(usart->SR & USART_SR_RXNE)) { }
		*buf++ = usart->DR;
	}
}

void *usart_dma_address(int num) {
	return (void *)&usarts[num]->DR; // discard volatile, doesn't matter cause we're just writing it to a DMA register
}
