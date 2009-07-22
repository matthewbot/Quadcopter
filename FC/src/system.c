#include "system.h"
#include "drivers/panel.h"
#include "drivers/stdio.h"
#include "peripherals/gpio.h"
#include "peripherals/rcc.h"
#include "peripherals/nvic.h"
#include "peripherals/adc.h"
#include <stdio.h>

void system_init() {
	gpio_init();
	panel_init();
	panel_set_status(PANEL_STATUS_BOOT);
	
	rcc_init();
	nvic_init();
	adc_init();

	stdio_init();
	
	panel_set_status(PANEL_STATUS_READY);
}
