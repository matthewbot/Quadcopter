#include "Timer.h"
#include <stmos/crt/nvic.h>
#include <cstddef>
#include <stm32f10x.h>

using namespace stmos;

static TIM_TypeDef *const timers[] = { 0, TIM1, TIM2, TIM3, TIM4 };
static void tim_handler(int num);

static Callback *overflowcallbacks[4];

template <int num> static void tim_handler_wrapper() {
	tim_handler(num);
}

Timer::Timer(int num, int pri) : num(num) {
	int interrupt=0;
	nvic_handler handler=NULL;
	switch (num) {
		case 1:
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			interrupt = TIM1_CC_IRQn;
			handler = tim_handler_wrapper<1>;
			break;
			
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
			AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_PARTIALREMAP2;
			interrupt = TIM2_IRQn;
			handler = tim_handler_wrapper<2>;
			break;
			
		case 3:
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
			AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_FULLREMAP;
			interrupt = TIM3_IRQn;
			handler = tim_handler_wrapper<3>;
			break;
			
		case 4:
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			interrupt = TIM4_IRQn;
			handler = tim_handler_wrapper<4>;
			break;
	}
	
	nvic_set_handler(interrupt, handler);
	nvic_set_priority(interrupt, pri);
	nvic_set_enabled(interrupt, true);
}

void Timer::setTickTime(unsigned int cycles) {
	TIM_TypeDef *tim = timers[num];
	
	tim->PSC = cycles;
}

void Timer::setDirection(Direction dir) {
	TIM_TypeDef *tim = timers[num];

	tim->CR1 |= ((int)dir << 4);
}

void Timer::setOverflow(uint16_t overflow) {
	TIM_TypeDef *tim = timers[num];
	
	tim->ARR = overflow;	
}

uint16_t Timer::getOverflow() const {
	TIM_TypeDef *tim = timers[num];
	
	return tim->ARR;
}

void Timer::setOverflowCallback(Callback &callback) {
	TIM_TypeDef *tim = timers[num];
	
	overflowcallbacks[num-1] = &callback;
	tim->DIER |= TIM_DIER_UIE;
}

void Timer::start() {
	TIM_TypeDef *tim = timers[num];
	
	tim->CR1 |= TIM_CR1_CEN;
}

void Timer::stop() {
	TIM_TypeDef *tim = timers[num];
	
	tim->CR1 &= ~TIM_CR1_CEN;
}

uint16_t Timer::read() {
	TIM_TypeDef *tim = timers[num];
	
	return tim->CNT;
}

static const IOPin::PortPin channelpins[4][4] = {
	{ {IOPin::PORT_A, 8}, {IOPin::PORT_A, 9}, {IOPin::PORT_A, 10}, {IOPin::PORT_A, 11} },
	{ {IOPin::PORT_A, 0}, {IOPin::PORT_A, 1}, {IOPin::PORT_B, 10}, {IOPin::PORT_B, 11} }, // also remap
	{ {IOPin::PORT_C, 6}, {IOPin::PORT_C, 7}, {IOPin::PORT_C, 8}, {IOPin::PORT_C, 9} }, // actually the remap position. At the moment tim3 is always remapped
	{ {IOPin::PORT_B, 5}, {IOPin::PORT_B, 6}, {IOPin::PORT_B, 7}, {IOPin::PORT_B, 8} }
};

static Callback *callbacks[4][4];

TimerChannel::TimerChannel(int timernum, int num, IOPin::Mode mode)
: timernum(timernum), num(num), pin(channelpins[timernum-1][num-1], mode, IOPin::NONE, true) {
	TIM_TypeDef *tim = timers[timernum];
	
	tim->CCER &= ~(0x000F << (num-1)*4); // clear CCER bits
	
	switch (num) { // clear CCMR bits
		case 1:
			tim->CCMR1 &= ~0x00FF;
			break;
			
		case 2:
			tim->CCMR1 &= ~0xFF00;
			break;
			
		case 3:
			tim->CCMR2 &= ~0x00FF;
			break;
			
		case 4:
			tim->CCMR2 &= ~0xFF00;
			break;	
	}
}

void TimerChannel::setCCR(uint16_t ccr) {
	TIM_TypeDef *tim = timers[timernum];
	
	switch (num) {
		case 1:
			tim->CCR1 = ccr;
			
		case 2:
			tim->CCR2 = ccr;
		
		case 3:
			tim->CCR3 = ccr;
		
		case 4:
			tim->CCR4 = ccr;
	}
}

uint16_t TimerChannel::getCCR() const {
	TIM_TypeDef *tim = timers[timernum];
	
	switch (num) {
		case 1:
			return tim->CCR1;
			
		case 2:
			return tim->CCR2;
		
		case 3:
			return tim->CCR3;
		
		case 4:
			return tim->CCR4;
	}
	
	return 0;
}

void TimerChannel::setCallback(Callback &callback) {
	TIM_TypeDef *tim = timers[timernum];

	callbacks[timernum-1][num-1] = &callback;
	tim->DIER |= (1 << num);
}

void TimerChannel::setEnabled(bool enabled) {
	TIM_TypeDef *tim = timers[timernum];
	
	int mask = 1 << (num-1)*4;
	if (enabled)
		tim->CCER |= mask;
	else
		tim->CCER &= ~mask;
}

void TimerChannel::setIRQEnabled(bool enabled) {
	TIM_TypeDef *tim = timers[timernum];
	
	int mask = 1 << num;
	if (enabled)
		tim->DIER |= mask;
	else
		tim->DIER &= ~mask;
}

void TimerChannel::setPolarity(bool pol) {
	TIM_TypeDef *tim = timers[timernum];

	int mask = 1 << (1 + 4*(num-1));

	if (pol) 
		tim->CCER |= mask;
	else
		tim->CCER &= ~mask;
}

InputCaptureTimerChannel::InputCaptureTimerChannel(int timernum, int num) : TimerChannel(timernum, num, IOPin::INPUT) {
	TIM_TypeDef *tim = timers[timernum];
	
	switch (num) {
		case 1:
			tim->CCMR1 |= TIM_CCMR1_CC1S_0; // set input mode
			break;
			
		case 2:
			tim->CCMR1 |= TIM_CCMR1_CC2S_0; // set input mode
			break;
			
		case 3:
			tim->CCMR2 |= TIM_CCMR2_CC3S_0; // set input mode
			break;
			
		case 4:
			tim->CCMR2 |= TIM_CCMR2_CC4S_0; // set input mode
			break;	
	}
}

void InputCaptureTimerChannel::setFilter(Filter filter) {
	TIM_TypeDef *tim = timers[timernum];

	switch (num) {
		case 1:
			tim->CCMR1 &= ~TIM_CCMR1_IC1F;
			tim->CCMR1 |= filter << 4;
			break;
			
		case 2:
			tim->CCMR1 &= ~TIM_CCMR1_IC2F;
			tim->CCMR1 |= filter << 12;
			break;
			
		case 3:
			tim->CCMR2 &= ~TIM_CCMR2_IC3F;
			tim->CCMR2 |= filter << 4;
			break;
			
		case 4:
			tim->CCMR2 &= ~TIM_CCMR2_IC4F;
			tim->CCMR2 |= filter << 12;
			break;	
	}	
}

OutputCompareTimerChannel::OutputCompareTimerChannel(int timernum, int num) : TimerChannel(timernum, num, IOPin::OUTPUT) {
	// no initialization to do, TimerChannel sets all CCMR bits to 0s, which is the output configuration
}

void OutputCompareTimerChannel::setMode(Mode mode) {
	TIM_TypeDef *tim = timers[timernum];

	switch (num) {
		case 1:
			tim->CCMR1 &= ~TIM_CCMR1_OC1M;
			tim->CCMR1 |= mode << 4;
			break;
			
		case 2:
			tim->CCMR1 &= ~TIM_CCMR1_OC2M;
			tim->CCMR1 |= mode << 12;
			break;
			
		case 3:
			tim->CCMR2 &= ~TIM_CCMR2_OC3M;
			tim->CCMR2 |= mode << 4;
			break;
			
		case 4:
			tim->CCMR2 &= ~TIM_CCMR2_OC4M;
			tim->CCMR2 |= mode << 12;
			break;	
	}
}

extern "C" {
#include <stmos/crt/debug.h>
}

static void tim_handler(int num) {
	TIM_TypeDef *tim = timers[num];
	uint16_t sr = tim->SR;
	Callback *callback = NULL;
	
	if (sr & TIM_SR_UIF) {
		callback = overflowcallbacks[num-1];
		tim->SR &= ~TIM_SR_UIF;
	} else if (sr & TIM_SR_CC1IF) {
		callback = callbacks[num-1][0];
		tim->SR &= ~TIM_SR_CC1IF;
	} else if (sr & TIM_SR_CC2IF) {
		callback = callbacks[num-1][1];
		tim->SR &= ~TIM_SR_CC2IF;
	} else if (sr & TIM_SR_CC3IF) {
		callback = callbacks[num-1][2];
		tim->SR &= ~TIM_SR_CC3IF;
	} else if (sr & TIM_SR_CC4IF) {
		callback = callbacks[num-1][3];
		tim->SR &= ~TIM_SR_CC4IF;
	}
	
	if (callback != NULL)
		callback->call();
}

