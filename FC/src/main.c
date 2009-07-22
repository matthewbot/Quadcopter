#include <stdio.h>
#include <stdbool.h>
#include "drivers/micromag.h"
#include "drivers/imu.h"
#include "peripherals/dma.h"
#include "peripherals/gpio.h"
#include "stm32f10x.h"
#include "system.h"

static void pause() {
	unsigned long val;
	for (val=0;val<5000000;val++) { asm("nop"); }
}

extern int vexrc_capture;

int main(int argc, char **argv) {
	system_init();
	
	printf("Waiting for pin\n");
	while (!gpio_input(GPIO_PORT_B, 10)) { }
	
	while (true) {
		pause();
		printf("Timer: %d\n Capture: %d\n CCR3: %d\n Pin: %d\n", (int)TIM2->CNT, vexrc_capture, (int)TIM2->CCR3, gpio_input(GPIO_PORT_B, 10));
	}
	
	/*
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
	*/
	return 0;
}


