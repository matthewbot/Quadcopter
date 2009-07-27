#include <stdio.h>
#include <stdbool.h>
#include "drivers/micromag.h"
#include "drivers/imu.h"
#include "drivers/panel.h"
#include "drivers/vexrc.h"
#include "peripherals/dma.h"
#include "peripherals/gpio.h"
#include "crt/status.h"
#include "stm32f10x.h"
#include "system.h"
#include <string.h>

static void pause() {
	unsigned long val;
	for (val=0;val<5000000;val++) { asm("nop"); }
}

extern int vexrc_capture;
extern unsigned int gpioa, gpiob;

int main(int argc, char **argv) {
	system_init();

	while (true) {
		pause();
	
		struct vexrc_channels chan = vexrc_get_channels();
		printf("Synced %d\n", (int)chan.synced);
		int i;
		for (i=0; i < 6; i++) {
			printf("%u ", (unsigned int)chan.channels[i]);
		}
		printf("\n");
	}
	
	return 0;
}


