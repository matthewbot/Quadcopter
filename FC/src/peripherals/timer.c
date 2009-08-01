#include "peripherals/timer.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Timer 1 not supported yet

#define TIM_CCMR_INPUT_IC1_TI1 TIM_CCMR1_CC1S_0

static TIM_TypeDef *const timers[] = { 0, TIM1, TIM2, TIM3, TIM4 };
static void timer_irq_handler_1();
static void timer_irq_handler_2();
static void timer_irq_handler_3();
static void timer_irq_handler_4();
static void timer_irq_handler(int timer);
static void timer_irq_overflow(int timer);

static void setup_ccmr(TIM_TypeDef *tim, int channel, uint16_t val);
static void setup_ccer(TIM_TypeDef *tim, int channel, uint16_t val);
static void setup_dier(TIM_TypeDef *tim, int channel, bool enable);

struct channel_callback_config {
	bool input;
	union {
		timer_capture_callback capture;
		timer_output_callback output;
		void *ptr;
	} callback;
};

#define MAX_OVERFLOW_CALLBACKS 4
static timer_overflow_callback overflowcallbacks[4][MAX_OVERFLOW_CALLBACKS];
static struct channel_callback_config chancallbacks[4][4];

void timer_init() {
	nvic_register_handler(TIM1_CC_IRQn, timer_irq_handler_1, false);
	nvic_register_handler(TIM2_IRQn, timer_irq_handler_2, false);
	nvic_register_handler(TIM3_IRQn, timer_irq_handler_3, false);
	nvic_register_handler(TIM4_IRQn, timer_irq_handler_4, false);
}

void timer_setup(int timer, int microsec, uint16_t maxval, enum timer_direction dir) {
	TIM_TypeDef *tim = timers[timer];
	tim->PSC = 72 * microsec;
	tim->ARR = maxval;
	tim->CR1 = TIM_CR1_CEN | (dir << 4);
	tim->DIER |= TIM_DIER_UIE;
	
	if (timer > 1)
		nvic_enable_interrupt(TIM2_IRQn + timer-2);
	else
		nvic_enable_interrupt(TIM1_CC_IRQn);
}

void timer_add_overflow_callback(int timer, timer_overflow_callback callback) {
	timer_overflow_callback *callbacks = overflowcallbacks[timer];
	
	int i;
	for (i=0; i<MAX_OVERFLOW_CALLBACKS;i++) {
		if (callbacks[i] == NULL) {
			callbacks[i] = callback;
			break;
		}
	}
}

void timer_channel_setup_ic(int timer, int channel, enum timer_ic_filter filter, enum timer_ic_edge edge, timer_capture_callback callback) {
	TIM_TypeDef *tim = timers[timer];
	
	struct channel_callback_config *chanconf = &chancallbacks[timer-1][channel-1];
	chanconf->callback.ptr = NULL;
	
	setup_ccmr(tim, channel, TIM_CCMR_INPUT_IC1_TI1 | (filter << 4)); // set CCMR register
	setup_ccer(tim, channel, TIM_CCER_CC1E | (edge << 1)); // compute our 4-bit value

	chanconf->input = true;
	chanconf->callback.capture = callback;
	
	setup_dier(tim, channel, callback != 0);
}

void timer_channel_setup_oc(int timer, int channel, enum timer_oc_mode mode, timer_output_callback callback, uint16_t ccr) {
	TIM_TypeDef *tim = timers[timer];
	
	struct channel_callback_config *chanconf = &chancallbacks[timer-1][channel-1];
	chanconf->callback.ptr = NULL;
	
	setup_ccmr(tim, channel, TIM_CCMR1_OC1PE | (mode << 4));
	setup_ccer(tim, channel, TIM_CCER_CC1E);
	
	chanconf->input = false;
	chanconf->callback.capture = callback;
	
	setup_dier(tim, channel, callback != 0);
		
	timer_channel_set_ccr(timer, channel, ccr);
}

void timer_channel_set_ccr(int timer, int channel, uint16_t ccr) {
	TIM_TypeDef *tim = timers[timer];
	
	switch (channel) {
		case 1:
			tim->CCR1 = ccr;
			break;
			
		case 2:
			tim->CCR2 = ccr;
			break;
			
		case 3:
			tim->CCR3 = ccr;
			break;
			
		case 4:
			tim->CCR4 = ccr;
			break;
	}
}

static void setup_ccmr(TIM_TypeDef *tim, int channel, uint16_t val) {
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

static void setup_ccer(TIM_TypeDef *tim, int channel, uint16_t val) {
	uint16_t mask = 0x0F; // mask for our bits
	
	const int shiftamt = (channel-1)*4;
	val <<= shiftamt; // shift our value to put it in the correct channel
	mask <<= shiftamt; // shift our mask accordingly
	
	uint16_t CCER = tim->CCER; // read original value
	CCER &= ~mask; // clear our bits
	CCER |= val; // set our bits
	tim->CCER = CCER; // write register back
}

static void setup_dier(TIM_TypeDef *tim, int channel, bool enable) {
	if (enable)
		tim->DIER |= (1 << channel);
	else
		tim->DIER &= ~(1 << channel);
}

static void timer_irq_handler(int timer) {
	TIM_TypeDef *tim = timers[timer];
	uint16_t sr = tim->SR;
	tim->SR = 0;

	struct channel_callback_config *callconf;
	uint16_t CCR;

	if (sr & TIM_SR_UIF) {
		timer_irq_overflow(timer);
		return;
	} else if (sr & TIM_SR_CC1IF) {
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
	} else {
		return;
	}
	
		
	if (callconf->callback.ptr == NULL) {
		return;
	}
		
	if (callconf->input) {
		callconf->callback.capture(CCR);
	} else {
		callconf->callback.output();
	}
}

static void timer_irq_overflow(int timer) {
	int channel;
	for (channel=0; channel<4; channel++) {
		struct channel_callback_config *callconf = &chancallbacks[timer-1][channel];
		if (callconf->input && callconf->callback.ptr)
			callconf->callback.capture(TIMER_IC_OVERFLOW);
	}
	
	int i;
	for (i=0; i<MAX_OVERFLOW_CALLBACKS; i++) {
		timer_overflow_callback callback = overflowcallbacks[timer-1][i];
		if (callback)
			callback();
	}
}

static void timer_irq_handler_1() {
	timer_irq_handler(1);
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
