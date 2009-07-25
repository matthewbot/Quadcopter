#include "system.h"
#include "drivers/panel.h"
#include "drivers/stdio.h"
#include "drivers/micromag.h"
#include "drivers/imu.h"
#include "drivers/vexrc.h"
#include "peripherals/gpio.h"
#include "peripherals/rcc.h"
#include "peripherals/nvic.h"
#include "peripherals/adc.h"
#include "peripherals/timer.h"
#include <stdio.h>

void system_init() {
	gpio_init();
	panel_init();
	panel_set_status(PANEL_STATUS_BOOT);
	
	rcc_init();
	nvic_init();
	adc_init();
	timer_init();

	stdio_init();
	micromag_init();
	imu_init();
	vexrc_init();
	
	puts("System Initialized");
		
	panel_set_status(PANEL_STATUS_READY);
}
