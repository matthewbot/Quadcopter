#include "peripherals/spi.h"
#include "stm32f10x.h"
#include <stddef.h>

static SPI_TypeDef *const spis[] = { 0, SPI1, SPI2 };

void spi_setup(int num, enum spi_baud_div baud, bool dma) {
	SPI_TypeDef *spi = spis[num];

	spi->CR2 = SPI_CR2_SSOE | (dma ? SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN : 0);	
	spi->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | (baud << 3);
}

void spi_disable(int num) {
	spis[num]->CR1 &= ~SPI_CR1_SPE;
}

#include <stdio.h>

void spi_send_receive(int num, uint8_t *data_in, const uint8_t *data_out, size_t len) {
	SPI_TypeDef *spi = spis[num];
	
	while (len--) {
		if (data_out != NULL)
			spi->DR = *data_out++;
		else
			spi->DR = 0;
			
		while (!(spi->SR & SPI_SR_TXE)) { }
		while (!(spi->SR & SPI_SR_RXNE)) { }
		
		uint32_t got = spi->DR;
		if (data_in != NULL)
			*data_in++ = got;
	}
}

void *spi_dma_address(int num) {
	return (void *)&spis[num]->DR;
}
