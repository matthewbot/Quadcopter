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
#include "stm32f10x.h"
#include <stdio.h>
#include <stdint.h>

extern uint32_t gpiob;


#define CNF_ANALOG 0
#define CNF_INPUT_FLOAT 1
#define CNF_INPUT_PULL 2

#define CNF_PUSH_PULL 0
#define CNF_OPEN_DRAIN 1
#define CNF_AF_PUSH_PULL 2
#define CNF_AF_OPEN_DRAIN 3

#define MODE_INPUT 0
#define MODE_OUTPUT_10MHZ 1
#define MODE_OUTPUT_2MHZ 2
#define MODE_OUTPUT_50MHZ 3

#define PINLO(num, cnf, mode) (cnf << ((num)*4 + 2)) | (mode << ((num)*4))
#define PINHI(num, cnf, mode) PINLO(num-8, cnf, mode)

void system_init() {
	rcc_init();
	gpio_init();
	nvic_init();
		
	panel_init();
	panel_set_status(PANEL_STATUS_BOOT);

	adc_init();
	timer_init();

	stdio_init();
	micromag_init();
	imu_init();
	vexrc_init();
		
	panel_set_status(PANEL_STATUS_READY);
	puts("System Initialized");
}
