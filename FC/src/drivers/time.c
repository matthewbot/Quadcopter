#include "drivers/time.h"
#include "peripherals/systick.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"

static void systick_irq_handler();

static volatile unsigned long milliseconds;

void time_init() {
	nvic_register_handler(SysTick_IRQn, systick_irq_handler);
	systick_setup(1000);
}

unsigned long time_get() { 
	return milliseconds;
}

void time_sleep(unsigned long msec) {
	unsigned long start = time_get();
	while (time_get() - start < msec) { }
}

static void systick_irq_handler() {
	milliseconds++;
}
