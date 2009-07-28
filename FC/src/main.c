#include <stdio.h>
#include <stdbool.h>
#include "drivers/micromag.h"
#include "drivers/imu.h"
#include "drivers/panel.h"
#include "drivers/vexrc.h"
#include "drivers/time.h"
#include "peripherals/dma.h"
#include "peripherals/gpio.h"
#include "crt/status.h"
#include "stm32f10x.h"
#include "system.h"
#include <string.h>

static void pause() {
	time_sleep(500);
}

int main(int argc, char **argv) {
	system_init();

	printf("Query %d\n", (int)micromag_sample(MICROMAG_AXIS_X, MICROMAG_PERIOD_4096));

	micromag_scan(MICROMAG_PERIOD_4096);

	while (true) {
		pause();
		struct micromag_scan_results results = micromag_get_scan();
		
		printf("%d %d %d\n", (int)results.x, (int)results.y, (int)results.z);
	}
}


