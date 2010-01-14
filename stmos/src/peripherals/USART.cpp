#include "USART.h"
#include <stmos/util/Task.h>
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>
#include <cstring>
#include <cstdio>
#include <cstdarg>

using namespace stmos;

static USART_TypeDef *const usarts[] = { USART1, USART2, USART3 };

static struct RecvBuf {
	char data[15];
	uint8_t len;
} recvbufs[3];

struct usart_config {
	IOPin::Port port;
	IOPin::Pin txpin, rxpin;
};

static const usart_config configs[3] = {
	{IOPin::PORT_A, 9, 10},
	{IOPin::PORT_A, 2, 3},
	{IOPin::PORT_B, 10, 11}
};

static void usart1_irq();
static void usart2_irq();
static void usart3_irq();

static void (*const usart_irqs[])() = { usart1_irq, usart2_irq, usart3_irq };

USART::USART(int num, unsigned int baud) 
: num(num), printfbuf(NULL),
  txpin(configs[num-1].port, configs[num-1].txpin, IOPin::OUTPUT, IOPin::NONE, true), 
  rxpin(configs[num-1].port, configs[num-1].rxpin, IOPin::INPUT, IOPin::NONE, true),
  recvbuf(NULL) {
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
	usart->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	
	int irq = USART1_IRQn + num - 1;
	nvic_set_handler(irq, usart_irqs[num-1]);
	nvic_set_enabled(irq, true);
}

USART::~USART() {
	delete printfbuf;
	delete recvbuf;
}

void USART::print(const char *str) {
	send((const uint8_t *)str, strlen(str));
}

#define PRINTF_MAXSIZE 100

void USART::printf(const char *fmt, ...) {
	if (!printfbuf)
		printfbuf = new char [PRINTF_MAXSIZE];
	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(printfbuf, PRINTF_MAXSIZE, fmt, ap);
	va_end(ap);
	
	print(printfbuf);
}

char USART::getch() {
	char ch;
	while (receive((uint8_t *)&ch, 1) == 0) { Task::sleep(1); }
	
	return ch;
}

size_t USART::getline(char *buf, size_t bufsize) {
	size_t count=0;
	bufsize--; // take away one just to hold the NULL
	while (bufsize--) {
		count++;
		if ((*buf++ = getch()) == '\r')
			break;
	}
	*buf++ = '\0';
	
	return count;
}

void USART::send(const uint8_t *buf, size_t len) {
	USART_TypeDef *usart = usarts[num-1];
	
	while (len--) {		
		while (!(usart->SR & USART_SR_TXE)) { }
		usart->DR = *buf++;
	}
	
	while (!(usart->SR & USART_SR_TC)) { }
}

int USART::receive(uint8_t *buf, size_t maxlen) {
	RecvBuf *recvbuf = &recvbufs[num-1];
	int irq = USART1_IRQn + num - 1;
	
	nvic_set_enabled(irq, false);
	
	unsigned int len = recvbuf->len;
	if (len > maxlen)
		len = maxlen;
		
	if (len > 0) {
		memcpy(buf, recvbuf->data, len);
		int remaining = recvbuf->len - len;
		
		if (remaining > 0) {
			memmove(recvbuf->data, &recvbuf->data[len], remaining);
			recvbuf->len -= len;
		} else
			recvbuf->len = 0;
	}
	
	nvic_set_enabled(irq, true);
	return len;
}

static void usart_irq(int num) {
	USART_TypeDef *usart = usarts[num-1];
	RecvBuf *recvbuf = &recvbufs[num-1];
	
	if (usart->SR & USART_SR_RXNE) 
		recvbuf->data[recvbuf->len++] = usart->DR;
}

static void usart1_irq() { usart_irq(1); }
static void usart2_irq() { usart_irq(2); }
static void usart3_irq() { usart_irq(3); }

