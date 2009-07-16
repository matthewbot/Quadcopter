#include "system.h"
#include "drivers/gpio.h"
#include "drivers/panel.h"
#include "drivers/rcc.h"
#include <stdio.h>

void system_init() {
	rcc_init();
	gpio_init();
	panel_init();
	
	printf("FC initialized\n");
}
