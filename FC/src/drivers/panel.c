#include "drivers/panel.h"
#include "peripherals/gpio.h"
#include "stm32f10x.h"
#include <stdbool.h>

void panel_init() {
	panel_set_status(PANEL_STATUS_BOOT);
}

struct panel_lights {
	bool green : 1, red : 1, yellowA : 1, yellowB : 1;
};


void panel_set_status(enum panel_status status) {
	static const struct panel_lights lights_config[] = {	
		{true, true, false, false},   // PANEL_STATUS_BOOT 
		{false, false, true, true},  // PANEL_STATUS_TEST 
		{true, false, true, false},  // PANEL_STATUS_CALIBRATING 
		{true, false, false, false}, // PANEL_STATUS_READY 
		{false, true, false, false}  // PANEL_STATUS_FAULT 
	};
	
	struct panel_lights config = lights_config[status];
	gpio_output(GPIO_PORT_A, 14, config.green);
	gpio_output(GPIO_PORT_B, 3, config.red);
	gpio_output(GPIO_PORT_A, 13, config.yellowA);
	gpio_output(GPIO_PORT_A, 15, config.yellowB);
}
