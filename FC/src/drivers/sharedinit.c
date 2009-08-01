#include "drivers/sharedinit.h"
#include "peripherals/nvic.h"
#include "peripherals/timer.h"
#include "stm32f10x.h"

static const int pri_low = 0, pri_med = 5, pri_high = 10, pri_max = 15;

void sharedinit_init() {
	nvic_set_priority(SysTick_IRQn, pri_max); // Used to keep system time. Interrupt is very tiny, occurs only once per ms
	nvic_set_priority(EXTI9_5_IRQn, pri_low); // micromag drdy line. micromag waits indefinitely for us to read back its data, so low priority
	nvic_set_priority(TIM2_IRQn, pri_high); // used to time external devices that don't wait for us, like the RC receiver and sonar
	nvic_set_priority(DMA1_Channel4_IRQn, pri_med); // handles receive buffer overruns on serial port
	
	timer_setup(2, 1000 / TIMER2_TPMS, TIMER2_MAXVAL, TIMER_DIRECTION_UP); // 10 microsecond or .01 ms per tick. Overflow at 40ms
	timer_setup(4, 1000 / TIMER4_TPMS, TIMER4_MAXVAL, TIMER_DIRECTION_UP); // Overflow at 5ms. Used to time adc
}
