#ifndef FC_PERIPHERALS_EXTI_H
#define FC_PERIPHERALS_EXTI_H

#include "peripherals/gpio.h"

enum exti_edge {
	EXTI_EDGE_RISING = 0x01,
	EXTI_EDGE_FALLING = 0x02,
	EXTI_EDGE_BOTH = 0x03
};

typedef void (*exti_handler)();

void exti_init();
void exti_setup(int exti, enum gpio_port port, enum exti_edge edge, exti_handler handler);

#endif
