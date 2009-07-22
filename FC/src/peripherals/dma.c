#include "peripherals/dma.h"
#include "stm32f10x.h"
#include <stdint.h>

static DMA_Channel_TypeDef *const channels[] = { 
	0, DMA1_Channel1, DMA1_Channel2, DMA1_Channel3, DMA1_Channel4, DMA1_Channel5, DMA1_Channel6, DMA1_Channel7
};

void dma_configure(int chnum, enum dma_direction dir, enum dma_priority pri, int memsize, int persize, enum dma_options opts) {
	uint32_t val = opts;
	
	if (dir == DMA_DIR_MEM_TO_PERIPHERAL)
		val |= DMA_CCR1_DIR;
		
	val |= pri << 12;	
		
	if (memsize == 16)
		val |= DMA_CCR1_MSIZE_0;
	else if (memsize == 32)
		val |= DMA_CCR1_MSIZE_1;
		
	if (persize == 16)
		val |= DMA_CCR1_PSIZE_0;
	else if (memsize == 32)
		val |= DMA_CCR1_MSIZE_1;
		
	channels[chnum]->CCR = val;
}

void dma_start(int chnum, void *mem, void *periph, size_t count) {
	DMA_Channel_TypeDef *ch = channels[chnum];
	ch->CCR &= ~DMA_CCR1_EN;
	ch->CNDTR = count;
	ch->CPAR = (uint32_t)periph;
	ch->CMAR = (uint32_t)mem;
	ch->CCR |= DMA_CCR1_EN;
}

void dma_disable(int chnum) {
	channels[chnum]->CCR &= ~DMA_CCR1_EN;
}

void dma_enable(int chnum) {
	channels[chnum]->CCR |= DMA_CCR1_EN;
}

size_t dma_get_remaining(int chnum) {
	return channels[chnum]->CNDTR;
}
