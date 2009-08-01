#include "drivers/time.h"
#include "peripherals/systick.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"
#include <stdio.h>

static void systick_irq_handler();

static volatile unsigned long milliseconds;

void time_init() {
	systick_setup(1000);
	nvic_register_handler(SysTick_IRQn, systick_irq_handler, false);
}

unsigned long time_get() { 
	return milliseconds;
}

void time_sleep(unsigned long msec) {
	unsigned long start = time_get();
	while (time_get() - start < msec) { }
}

struct time_highres time_get_highres() {
	struct time_highres ret;
	
	ret.cycles = systick_get();
	ret.msecs = milliseconds;
	
	return ret;
}

struct time_highres time_highres_subtract(struct time_highres timea, struct time_highres timeb) {
	struct time_highres ret;
	
	ret.msecs = timea.msecs - timeb.msecs;
	
	if (timea.cycles < timeb.cycles) {
		timea.cycles += 72000;
		timea.msecs--;
	}
	
	ret.cycles = timea.cycles - timeb.cycles;
	
	return ret;
}

void time_highres_print(struct time_highres time) {
	printf("Msec %u Cycles %u\n", (unsigned int)time.msecs, (unsigned int)time.cycles);
}

static void systick_irq_handler() {
	milliseconds++;
}
