#include "system.h"
#include "peripherals/gpio.h"
#include "peripherals/panel.h"
#include "peripherals/rcc.h"
#include <stdio.h>

void system_init() {
	rcc_init();
	gpio_init();
	panel_init();
	
	printf("FC initialized\n");
}
