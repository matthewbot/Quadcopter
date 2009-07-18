#include "drivers/stdio.h"
#include "peripherals/dma.h"
#include "peripherals/usart.h"
#include "peripherals/nvic.h"
#include <string.h>

// Called from newlib stubs! Don't make any syscalls!

#define DMA_TX 4
#define DMA_RX 5
#define DMA_RX_INTERRUPT (DMA_BASE_IRQn + DMA_RX)

#define BUF_SIZE 128

static uint8_t readbuf[BUF_SIZE], writebuf[BUF_SIZE];

static void readbuf_full_handler();
static void start_rx_dma(size_t bufinuse);

void stdio_init() {
	nvic_register_handler(DMA_RX_INTERRUPT, readbuf_full_handler);
	nvic_enable_interrupt(DMA_RX_INTERRUPT);
	dma_configure(DMA_TX, DMA_DIR_MEM_TO_PERIPHERAL, DMA_PRIORITY_LOW, 8, 8, DMA_OPTION_MEMORY_INCREMENT);
	dma_configure(DMA_RX, DMA_DIR_PERIPHERAL_TO_MEM, DMA_PRIORITY_LOW, 8, 8, DMA_OPTION_MEMORY_INCREMENT | DMA_OPTION_INTERRUPT);
	start_rx_dma(0);
	usart_setup(1, 115200, true);
}

size_t stdio_read(uint8_t *buf, size_t len) {
	dma_disable(DMA_RX);
	
	size_t avail;
	do {
		avail = sizeof(readbuf) - dma_get_remaining(DMA_RX);
	} while (avail == 0);
	size_t got = (avail > len) ? len : avail;
		
	size_t new_avail = avail - got;

	memcpy(buf, readbuf, got);
	if (new_avail > 0)
		memmove(readbuf, &readbuf[got], new_avail);
		
	start_rx_dma(new_avail);
	
	return got;
}

size_t stdio_write(const uint8_t *buf, size_t len) {
	if (len > sizeof(writebuf))
		len = sizeof(writebuf);
	
	while (dma_get_remaining(DMA_TX) > 0) { }
	
	memcpy(writebuf, buf, len);
	dma_start(DMA_TX, writebuf, usart_dma_address(1), len);
	
	return len;
}

#define DROP_BYTES 4
static void readbuf_full_handler() {
	memmove(readbuf, &readbuf[DROP_BYTES], sizeof(readbuf) - DROP_BYTES); // drop the first few bytes
	start_rx_dma(sizeof(readbuf) - DROP_BYTES); // restart DMA in the new free space
}
	
static void start_rx_dma(size_t bufinuse) {
	dma_start(DMA_RX, &readbuf[bufinuse], usart_dma_address(1), sizeof(readbuf) - bufinuse);
}
	
