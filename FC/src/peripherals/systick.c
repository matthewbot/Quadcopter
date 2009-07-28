#include "peripherals/systick.h"
#include "stm32f10x.h"

// why SysTick? why not STK? :/

void systick_setup(int microseconds) {
	SysTick->LOAD = microseconds*72 - 1;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;
}

uint32_t systick_get() {
	return SysTick->VAL;
}
