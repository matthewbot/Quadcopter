#include "peripherals/systick.h"
#include "stm32f10x.h"

// why SysTick? why not STK? :/

void systick_setup(int microseconds) {
	SysTick->LOAD = (microseconds-1)*72;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;
}
