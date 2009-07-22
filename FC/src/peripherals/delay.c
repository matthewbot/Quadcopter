#include "peripherals/delay.h"

void delay_cycles(unsigned long count) {
	while (count--) {
		asm("nop");
	}
}
