#include "SPI.h"
#include <stm32f10x.h>

using namespace stmos;

static const IOPinConfig::Port spiports[2] = { IOPinConfig::PORT_A, IOPinConfig::PORT_B };
static const IOPinConfig::Pin spibasepins[2] = { 4, 12 };
static SPI_TypeDef *const spis[] = { 0, SPI1, SPI2 };

SPI::SPI(int num, Baud baud)
: num(num),
  nss_pin(spiports[num-1], spibasepins[num-1], IOPinConfig::OUTPUT, IOPinConfig::NONE, true),
  sck_pin(spiports[num-1], spibasepins[num-1]+1, IOPinConfig::OUTPUT, IOPinConfig::NONE, true),
  miso_pin(spiports[num-1], spibasepins[num-1]+2, IOPinConfig::INPUT, IOPinConfig::NONE, true),
  mosi_pin(spiports[num-1], spibasepins[num-1]+3, IOPinConfig::OUTPUT, IOPinConfig::NONE, true) 
{
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
}

void SPI::sendReceive(const uint8_t *sendbuf, uint8_t *receivebuf, size_t len) {
	SPI_TypeDef *spi = spis[num];
	
	while (len--) {
		while (!(spi->SR & SPI_SR_TXE)) { }
	
		if (sendbuf)
			spi->DR = *sendbuf++;
		else
			spi->DR = 0;
		
		while (!(spi->SR & SPI_SR_RXNE)) { }
		uint32_t got = spi->DR;
		
		if (receivebuf) 
			*receivebuf++ = got;
	}
}

