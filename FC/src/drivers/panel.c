#include "drivers/panel.h"
#include "peripherals/gpio.h"
#include "stm32f10x.h"
#include <stdbool.h>

void panel_init() {
	panel_set_status(PANEL_STATUS_BOOT);
}

struct panel_lights {
	bool green : 1, yellow : 1, red : 1;
};


void panel_set_status(enum panel_status status) {
	static const struct panel_lights lights_config[] = {	
		{true, true, true},   // PANEL_STATUS_BOOT GYR
		{false, true, true},  // PANEL_STATUS_TEST YR
		{false, true, false}, // PANEL_STATUS_TEST_FINISHED Y
		{true, true, false},  // PANEL_STATUS_CALIBRATING GY
		{true, false, false}, // PANEL_STATUS_READY G
		{false, false, true}  // PANEL_STATUS_FAULT R
	};
	
	struct panel_lights config = lights_config[status];
	gpio_output(GPIO_PORT_A, 13, config.red);
	gpio_output(GPIO_PORT_A, 14, config.yellow);
	gpio_output(GPIO_PORT_A, 15, config.green);
}

void panel_set_autopilot(bool autopilot) {
	gpio_output(GPIO_PORT_C, 13, autopilot);
	gpio_output(GPIO_PORT_B, 3, autopilot);
}

void panel_set_positionlock(bool poslock) {
	gpio_output(GPIO_PORT_B, 3, poslock);
}
