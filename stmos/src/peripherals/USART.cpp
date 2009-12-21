#include "USART.h"
#include <stm32f10x.h>
#include <cstring>
#include <cstdio>
#include <cstdarg>

using namespace stmos;

static USART_TypeDef *const usarts[] = { USART1, USART2, USART3 };

struct usart_config {
	IOPin::Port port;
	IOPin::Pin txpin, rxpin;
};

static const usart_config configs[3] = {
	{IOPin::PORT_A, 9, 10},
	{IOPin::PORT_A, 2, 3},
	{IOPin::PORT_B, 10, 11}
};

USART::USART(int num, unsigned int baud) 
: num(num), printfbuf(NULL),
  txpin(configs[num-1].port, configs[num-1].txpin, IOPin::OUTPUT, IOPin::NONE, true), 
  rxpin(configs[num-1].port, configs[num-1].rxpin, IOPin::INPUT, IOPin::NONE, true) {
 	switch (num) {
 		case 1:
 			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
 			break;
 			
 		case 2:
 			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
 			break;
 		
 		case 3:
 			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
 			break;
 	}
 
	USART_TypeDef *usart = usarts[num-1];

	usart->CR1 = USART_CR1_UE;
	usart->BRR = ((num == 1) ? 72000000 : 36000000) / baud;
	usart->CR1 |= USART_CR1_TE | USART_CR1_RE;
}

void USART::print(const char *str) const {
	send((const uint8_t *)str, strlen(str));
}

#define PRINTF_MAXSIZE 50

void USART::printf(const char *fmt, ...) const {
	if (!printfbuf)
		printfbuf = new char [PRINTF_MAXSIZE];
	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(printfbuf, PRINTF_MAXSIZE, fmt, ap);
	va_end(ap);
	
	print(printfbuf);
}

void USART::send(const uint8_t *buf, size_t len) const {
	USART_TypeDef *usart = usarts[num-1];
	
	while (len--) {		
		while (!(usart->SR & USART_SR_TXE)) { }
		usart->DR = *buf++;
	}
	
	while (!(usart->SR & USART_SR_TC)) { }
}

void USART::receive(uint8_t *buf, size_t len) const {
	USART_TypeDef *usart = usarts[num-1];
	
	while (len--) {
		while (!(usart->SR & USART_SR_RXNE)) { }
		*buf++ = usart->DR;
	}
}

