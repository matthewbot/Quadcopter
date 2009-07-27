#include "crt/status.h"
#include "stm32f10x.h"
#include <stdbool.h>

void status_init() {
	RCC->APB2ENR = RCC_APB2ENR_IOPCEN; 
	GPIOC->CRH = 0x44434444; // make pin 12 open drain output 
	status_off();
}

void status_on() {
	GPIOC->BRR = (1 << 12); // make pin 12 high
}
	
void status_off() {
	GPIOC->BSRR = (1 << 12); // make pin 12 low
}


static void delay();

__attribute__ ((noreturn))
void status_blink_halt(int count) {
	__disable_irq();

	while (true) {
		int i;
		for (i=0;i<count;i++) {
			status_on();
			delay();
			status_off();
			delay();
		}
		
		delay();
		delay();
	}
}

static void delay() {
	unsigned long i;
	for (i=0;i<2000000;i++) { asm("nop"); }
}
