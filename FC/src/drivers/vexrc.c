#include "drivers/vexrc.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"
#include <stdint.h>

#define VEX_TIMR TIM2

#define TIM_CCMR2_IC3F_N8 (TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1)

int vexrc_capture=0;

static void vexrc_capture_handler();

void vexrc_init() {
	nvic_register_handler(TIM2_IRQn, vexrc_capture_handler);
	nvic_enable_interrupt(TIM2_IRQn);
	
	VEX_TIMR->PSC = 720; // count at .01 ms per tick (was 720)
	VEX_TIMR->CCMR2 = TIM_CCMR2_CC3S_0 | // input, IC3 on TI3
	                  TIM_CCMR2_IC3F_N8; // sample at CK_INT, 8 samples for valid transition
	VEX_TIMR->CCER = TIM_CCER_CC3E; // enable capture compare on channel 3
	VEX_TIMR->DIER = TIM_DIER_CC3IE; // enable interrupt on channel 3
	
	VEX_TIMR->CR1 = TIM_CR1_CEN;
}

static void vexrc_capture_handler() {
	vexrc_capture = 1;
}
