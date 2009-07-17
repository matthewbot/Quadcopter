#ifndef FC_PERIPHERALS_DMA_H
#define FC_PERIPHERALS_DMA_H

#include "stm32f10x.h"

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

void dma_configure(int chnum, enum dma_direction dir, enum dma_priority pri, int memsize, int persize, enum dma_options opts);
void dma_start(int chnum, void *mem, void *periph, unsigned short count);
void dma_disable(int chnum);
void dma_enable(int chnum);

#endif
