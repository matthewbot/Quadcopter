#ifndef FC_PERIPHERALS_DMA_H
#define FC_PERIPHERALS_DMA_H

#include "stm32f10x.h"
#include <stddef.h>

enum dma_interrupt_type {
	DMA_TRANSFER_COMPLETE,
	DMA_TRANSFER_HALF,
	DMA_TRANSFER_ERROR
};

enum dma_priority {
	DMA_PRIORITY_LOW = 0x00,
	DMA_PRIORITY_MEDIUM = 0x01,
	DMA_PRIORITY_HIGH = 0x02,
	DMA_PRIORITY_VERY_HIGH = 0x03
};

enum dma_direction {
	DMA_DIR_MEM_TO_PERIPHERAL,
	DMA_DIR_PERIPHERAL_TO_MEM
};

enum dma_options {
	DMA_OPTION_MEMORY_INCREMENT = DMA_CCR1_MINC,
	DMA_OPTION_PERIPHERAL_INCREMENT = DMA_CCR1_PINC,
	DMA_OPTION_CIRCULAR = DMA_CCR1_CIRC,
	DMA_OPTION_INTERRUPT = DMA_CCR1_TCIE
};

#define DMA_BASE_IRQn 10 // add the DMA channel number to this to get its IRQ ID

void dma_configure(int chnum, enum dma_direction dir, enum dma_priority pri, size_t size, enum dma_options opts);
void dma_start(int chnum, volatile void *mem, volatile void *periph, size_t count);
void dma_disable(int chnum);
void dma_enable(int chnum);
size_t dma_get_remaining(int chnum);
void dma_clear_interrupt(int chnum);

#endif
