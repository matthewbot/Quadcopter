#include "peripherals/timer.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Timer 1 not supported yet

#define TIM_CCMR_INPUT_IC1_TI1 TIM_CCMR1_CC1S_0

static TIM_TypeDef *const timers[] = { 0, 0, TIM2, TIM3, TIM4 };
static void timer_irq_handler_2();
static void timer_irq_handler_3();
static void timer_irq_handler_4();
static void timer_irq_handler(int timer);
static void timer_irq_overflow(int timer);

struct channel_callback_config {
	bool input;
	union {
		timer_capture_callback capture;
		timer_output_callback output;
		void *ptr;
	} callback;
};

static struct channel_callback_config chancallbacks[3][4];

void timer_init() {
	nvic_register_handler(TIM2_IRQn, timer_irq_handler_2);
	nvic_register_handler(TIM3_IRQn, timer_irq_handler_3);
	nvic_register_handler(TIM4_IRQn, timer_irq_handler_4);
}

void timer_setup(int timer, int microsec, uint16_t maxval, enum timer_direction dir) {
	TIM_TypeDef *tim = timers[timer];
	tim->PSC = 72 * microsec;
	tim->ARR = maxval;
	tim->CR1 = TIM_CR1_CEN | (dir << 4);
	
	if (timer > 1)
		nvic_enable_interrupt(TIM2_IRQn + timer-2);
}

void timer_channel_setup_ic(int timer, int channel, enum timer_ic_filter filter, enum timer_ic_edge edge, timer_capture_callback callback) {
	TIM_TypeDef *tim = timers[timer];
	
	{ // do nothing block to scope variables
		// CCMR1 has channel 1 in the lower 8 bits, channel 2 in the upper 8 bits
		// CCMR2 has channel 3 and 4
	
		uint16_t val = TIM_CCMR_INPUT_IC1_TI1 | (filter << 4); // compute the 8 bit value
		uint16_t mask = 0xFF; // mask for our bits
	
		__IO uint16_t *const CCMR_addr = channel < 3 ? &tim->CCMR1 : &tim->CCMR2; // channels 1 and 2 in CCMR1, 3 and 4 in CCMR2
	
		if ((channel & 1) == 0) { // if the channel is 2 or 4 (and thus in the upper 8 bits)
			val <<= 8; // shift the val and mask by 8
			mask <<= 8;
		}
	
		uint16_t CCMR = *CCMR_addr; // read original register value
		CCMR &= ~mask; // clear our bits
		CCMR |= val; // set our bits
		*CCMR_addr = CCMR; // write register back
	}
	
	{
		uint16_t val = TIM_CCER_CC1E | (edge << 1); // compute our 4-bit value
		uint16_t mask = 0x0F; // mask for our bits
		
		const int shiftamt = (channel-1)*4;
		val <<= shiftamt; // shift our value to put it in the correct channel
		mask <<= shiftamt; // shift our mask accordingly
		
		uint16_t CCER = tim->CCER; // read original value
		CCER &= ~mask; // clear our bits
		CCER |= val; // set our bits
		tim->CCER = CCER; // write register back
	}
	
	struct channel_callback_config *chanconf = &chancallbacks[timer-2][channel];
	chanconf->input = true;
	chanconf->callback.capture = callback;
}

static void timer_irq_handler(int timer) {
	TIM_TypeDef *tim = timers[timer];
	uint16_t sr = tim->SR;
		
	struct channel_callback_config *callconf;
	uint16_t CCR;

	if (sr & TIM_SR_CC1IF) {
		callconf = &chancallbacks[timer-1][0];
		CCR = tim->CCR1;
	} else if (sr & TIM_SR_CC2IF) {
		callconf = &chancallbacks[timer-1][1];
		CCR = tim->CCR2;
	} else if (sr & TIM_SR_CC3IF) {
		callconf = &chancallbacks[timer-1][2];
		CCR = tim->CCR3;
	} else if (sr & TIM_SR_CC4IF) {
		callconf = &chancallbacks[timer-1][3];
		CCR = tim->CCR4;
	} else if (sr & TIM_SR_UIF) {
		timer_irq_overflow(timer);
		return;
	}
		
	if (callconf->callback.ptr == NULL)
		return;
		
	if (callconf->input) {
		callconf->callback.capture(CCR);
	} else {
		tim->SR = 0; // clear interrupt 
		callconf->callback.output();
	}
}

static void timer_irq_overflow(int timer) {
	int channel;
	for (channel=0; channel<4; channel++) {
		struct channel_callback_config *callconf = &chancallbacks[timer-1][channel];
		if (callconf->input)
			callconf->callback.input(TIMER_IC_OVERFLOW);
	}
}

static void timer_irq_handler_2() {
	timer_irq_handler(2);
}

static void timer_irq_handler_3() {
	timer_irq_handler(3);
}

static void timer_irq_handler_4() {
	timer_irq_handler(4);
}
