#include "system.h"
#include "drivers/panel.h"
#include "drivers/stdio.h"
#include "drivers/micromag.h"
#include "drivers/imu.h"
#include "drivers/vexrc.h"
#include "drivers/time.h"
#include "peripherals/gpio.h"
#include "peripherals/rcc.h"
#include "peripherals/nvic.h"
#include "peripherals/adc.h"
#include "peripherals/timer.h"
#include "peripherals/exti.h"
#include <stdio.h>

void system_init() {
	rcc_init();
	gpio_init();
	nvic_init();
		
	panel_init();
	panel_set_status(PANEL_STATUS_BOOT);

	time_init();
	adc_init();
	timer_init();
	exti_init();

	stdio_init();
	micromag_init();
	imu_init();
	vexrc_init();
		
	panel_set_status(PANEL_STATUS_READY);
	puts("System Initialized");
}
