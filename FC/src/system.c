#include "system.h"
#include "drivers/panel.h"
#include "drivers/stdio.h"
#include "peripherals/gpio.h"
#include "peripherals/rcc.h"
#include "peripherals/nvic.h"
#include <stdio.h>

void system_init() {
	rcc_init();
	gpio_init();
	nvic_init();
	
	panel_init();
	stdio_init();
	
	puts("FC initialized\n");
}
