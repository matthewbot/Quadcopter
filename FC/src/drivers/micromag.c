#include "drivers/micromag.h"
#include "peripherals/spi.h"
#include "peripherals/gpio.h"
#include "peripherals/delay.h"
#include <stdint.h>

#define MICROMAG_SPI 2

static void micromag_reset();
static void micromag_set_reset_output(bool reset);
static bool micromag_get_drdy_input();

void micromag_init() {
	gpio_output(GPIO_PORT_B, 12, false);
	spi_setup(MICROMAG_SPI, SPI_BAUD_DIV_256, false);
}

#include <stdio.h>

uint16_t micromag_sample(enum micromag_axis axis, enum micromag_period period) {
	printf("Begin sample\n");
	
	micromag_reset();
	printf("Toggled reset. DRDY is %d\n", (int)micromag_get_drdy_input());
	
	uint8_t command_byte = axis | (period << 4);
	spi_send_receive(MICROMAG_SPI, NULL, &command_byte, 1);
	printf("Sent byte (%d)\n", (int)command_byte);
	
	while (!micromag_get_drdy_input()) { }
	printf("DRDY went high! Reading result\n");
	
	union {
		uint8_t bytes[2];
		uint16_t val;
	} result;
	spi_send_receive(MICROMAG_SPI, result.bytes, NULL, 2);
	
	printf("Complete! Got result: %d\n", (int)result.val);
	return result.val;
}

static void micromag_reset() {
	micromag_set_reset_output(false);
	delay_cycles(1000);
	micromag_set_reset_output(true);
	delay_cycles(1000);
	micromag_set_reset_output(false);
	delay_cycles(1000);
}

static bool micromag_get_drdy_input() {
	return gpio_input(GPIO_PORT_B, 8);
}

static void micromag_set_reset_output(bool reset) {
	gpio_output(GPIO_PORT_B, 5, reset);
}
