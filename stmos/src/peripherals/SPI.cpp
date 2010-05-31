#include "SPI.h"
#include <stmos/util/Lock.h>
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>

using namespace stmos;

static const IOPinConfig::Port spiports[2] = { IOPinConfig::PORT_A, IOPinConfig::PORT_B };
static const IOPinConfig::Pin spibasepins[2] = { 4, 12 };
static SPI_TypeDef *const spis[] = { 0, SPI1, SPI2 };

static SPI *objs[2];
template <int i> static void irq_callback() {
	objs[i]->irq();
}
static void (*const irq_callbacks[])() = {irq_callback<0>, irq_callback<1>};

#define IRQ(num) (SPI1_IRQn + (num) - 1)

SPI::SPI(int num, Baud baud)
: num(num),
  nss_pin(spiports[num-1], spibasepins[num-1], IOPinConfig::OUTPUT, IOPinConfig::NONE, true),
  sck_pin(spiports[num-1], spibasepins[num-1]+1, IOPinConfig::OUTPUT, IOPinConfig::NONE, true),
  miso_pin(spiports[num-1], spibasepins[num-1]+2, IOPinConfig::INPUT, IOPinConfig::NONE, true),
  mosi_pin(spiports[num-1], spibasepins[num-1]+3, IOPinConfig::OUTPUT, IOPinConfig::NONE, true) 
{
	objs[num-1] = this;
	
 	switch (num) {
 		case 1:
 			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
 			break;
 			
 		case 2:
 			RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
 			break;
 	}  
  
	SPI_TypeDef *spi = spis[num];
	
	spi->CR1 = 0;
	spi->CR2 = SPI_CR2_SSOE;
	spi->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | (baud << 3);
	
	int irq = IRQ(num);
	nvic_set_handler(irq, irq_callbacks[num-1]);
	nvic_set_enabled(irq, true);
}

SPI::~SPI() {
	nvic_set_enabled(IRQ(num-1), false);
}

void SPI::sendReceive(const uint8_t *sendbuf, uint8_t *recvbuf, size_t len) {
	SPI_TypeDef *spi = spis[num];

	Lock lock(mutex);
	CriticalSection crit(IRQ(num));
	
	this->sendbuf = sendbuf;
	this->sendlen = len;
	this->recvbuf = recvbuf;
	this->recvlen = len;

	spi->CR2 |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE;
	notifier.waitLeave(crit);
}

#include <stmos/crt/debug.h>

void SPI::irq() {
	SPI_TypeDef *spi = spis[num];
	
	uint32_t sr = spi->SR;

	if ((sr & SPI_SR_RXNE) && recvlen) {
		if (recvbuf) {
			*recvbuf++ = spi->DR;
		} else {
			(void)spi->DR;
		}
		
		if (--recvlen == 0) {
			spi->CR2 &= ~SPI_CR2_RXNEIE;
			register_irqcallback();
		}
	}
	
	if ((sr & SPI_SR_TXE) && sendlen) {
		if (sendbuf) {
			spi->DR = *sendbuf++;
		} else {
			spi->DR = 0;
		}
		
		if (--sendlen == 0)
			spi->CR2 &= ~SPI_CR2_TXEIE;
	}
}

void SPI::irqcallback() {
	notifier.notify();
}


