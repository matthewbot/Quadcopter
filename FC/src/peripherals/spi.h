#ifndef FC_PERIPHERALS_SPI_H
#define FC_PERIPHERALS_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum spi_baud_div {
	SPI_BAUD_DIV_2,
	SPI_BAUD_DIV_4,
	SPI_BAUD_DIV_8,
	SPI_BAUD_DIV_16,
	SPI_BAUD_DIV_32,
	SPI_BAUD_DIV_64,
	SPI_BAUD_DIV_128,
	SPI_BAUD_DIV_256
};

void spi_setup(int num, enum spi_baud_div baud, bool dma);
void spi_disable(int num);
void spi_send_receive(int num, uint8_t *data_in, const uint8_t *data_out, size_t len);
void *spi_dma_address(int num);

#endif
