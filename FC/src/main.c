#include <stdio.h>
#include <stdbool.h>
#include "drivers/micromag.h"
#include "drivers/imu.h"
#include "peripherals/dma.h"
#include "system.h"

static void pause() {
	unsigned long val;
	for (val=0;val<5000000;val++) { asm("nop"); }
}

int main(int argc, char **argv) {
	system_init();
	
	while (true) {
		pause();
		const uint16_t *raw = imu_read_raw();
		
		printf("Readings: ");
		
		int i;
		for (i=0; i < 6; i++) {
			printf("%d ", (int)raw[i]);
		}
		printf("\n");
	}

	return 0;
}


