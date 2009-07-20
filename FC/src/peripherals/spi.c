#include "peripherals/spi.h"
#include "stm32f10x.h"

static SPI_TypeDef *const spis[] = { 0, SPI1, SPI2 };

void spi_setup(int num, enum spi_baud_div baud, bool dma) {
	SPI_TypeDef *spi = spis[num];
	
	spi->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | (baud << 3);
	spi->CR2 = SPI_CR2_SSOE | (dma ? SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN : 0);
}

void spi_disable(int num) {
	spis[num]->CR1 &= ~SPI_CR1_SPE;
}

void spi_send_receive(int num, uint8_t *data_in, const uint8_t *data_out, size_t len) {
	SPI_TypeDef *spi = spis[num];
	
	while (len--) {
		spi->DR = *data_out++;
		while (!(spi->SR & SPI_SR_TXE)) { }
		while (!(spi->SR & SPI_SR_RXNE)) { }
		*data_in++ = spi->DR;
	}
}

void *spi_dma_address(int num) {
	return (void *)&spis[num]->DR;
}
