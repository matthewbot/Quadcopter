#include "DMA.h"
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>

using namespace stmos;

static DMA_Channel_TypeDef *const channels[] = { 
	0, DMA1_Channel1, DMA1_Channel2, DMA1_Channel3, DMA1_Channel4, DMA1_Channel5, DMA1_Channel6, DMA1_Channel7
};

static Callback *irqhandlers[7];
static void irqhandler();

DMA::DMA(int num) : num(num) {
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

void DMA::setup(Direction dir, Priority pri, size_t tsize) {
	DMA_Channel_TypeDef *chan = channels[num];

	uint32_t ccr=0;
	
	if (dir == DIRECTION_MEM_TO_PER)
		ccr |= DMA_CCR1_DIR;
		
	ccr |= pri << 12;
	
	if (tsize == 2)
		ccr |= DMA_CCR1_MSIZE_0 | DMA_CCR1_PSIZE_0;
	else if (tsize == 4)
		ccr |= DMA_CCR1_MSIZE_1 | DMA_CCR1_MSIZE_1;
		
	ccr |= DMA_CCR1_MINC;
	chan->CCR = ccr;
}

static int getirqnum(int num) { return DMA1_Channel1_IRQn + (num-1); }

void DMA::setIRQHandler(Callback &callback, int pri) {
	DMA_Channel_TypeDef *chan = channels[num];
	int irq = getirqnum(num);
	
	irqhandlers[num-1] = &callback;
	
	nvic_set_priority(irq, pri);
	nvic_set_handler(irq, irqhandler);
	nvic_set_enabled(irq, true);
	
	chan->CCR |= DMA_CCR1_TCIE;
}

void DMA::setIRQEnabled(bool enabled) {
	nvic_set_enabled(getirqnum(num), enabled);
}

void DMA::start(volatile void *mem, volatile void *periph, size_t count, bool circular) {
	DMA_Channel_TypeDef *chan = channels[num];
	
	suspend();
	if (circular)
		chan->CCR |= DMA_CCR1_CIRC;
	else
		chan->CCR &= ~DMA_CCR1_CIRC;
	chan->CMAR = (uint32_t)mem;
	chan->CPAR = (uint32_t)periph;
	chan->CNDTR = count;
	resume();
}

void DMA::suspend() {
	DMA_Channel_TypeDef *chan = channels[num];
	chan->CCR &= ~DMA_CCR1_EN;
}

void DMA::resume() {
	DMA_Channel_TypeDef *chan = channels[num];
	chan->CCR |= DMA_CCR1_EN;	
}

size_t DMA::getRemaining() const {
	DMA_Channel_TypeDef *chan = channels[num];
	return chan->CNDTR;
}

static void irqhandler() {
	int ch = nvic_get_activeirq() - DMA1_Channel1_IRQn;
	irqhandlers[ch]->call();
	DMA1->IFCR = 1 << (ch)*4;
}
	
