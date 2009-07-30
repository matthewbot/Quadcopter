#include <stdio.h>
#include <stdbool.h>
#include "drivers/micromag.h"
#include "drivers/analog.h"
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

	micromag_scan(MICROMAG_PERIOD_1024);

	while (true) {
		pause();
		struct micromag_scan_results results = micromag_get_scan();
		
		printf("Micromag %d %d %d\n", (int)results.x, (int)results.y, (int)results.z);
		struct vexrc_channels chan = vexrc_get_channels();
		if (chan.synced)
			printf("VexRC S ");
		else
			printf("VexRC NS ");
		int i;
		for (i=0; i<6; i++) {
			printf("%d ", (int)chan.channels[i]);
		}
		printf("\n");
		
		printf("Analog ");
		const volatile uint16_t *analog_raw = analog_get_raw();
		for (i=0; i<6; i++) {
			printf("%d ", (int)analog_raw[i]);
		}
		printf("\n");
	}
}


