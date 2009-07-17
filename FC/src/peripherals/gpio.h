#ifndef FC_DRIVERS_GPIO_H
#define FC_DRIVERS_GPIO_H

#include <stdbool.h>

enum gpio_port {
	GPIO_PORT_A, 
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D
};

void gpio_init(); // configures ports and AFs for our hardware
void gpio_output(enum gpio_port port, int pin, bool val);
bool gpio_input(enum gpio_port port, int pin);


#endif
