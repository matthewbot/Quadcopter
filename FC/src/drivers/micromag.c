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
	spi_setup(MICROMAG_SPI, SPI_BAUD_DIV_256, false);
}

int16_t micromag_sample(enum micromag_axis axis, enum micromag_period period) {
	micromag_reset();
	
	uint8_t command_byte = axis | (period << 4);
	spi_send_receive(MICROMAG_SPI, NULL, &command_byte, 1);
	
	while (!micromag_get_drdy_input()) { }
	
	uint8_t bytes[2];
	spi_send_receive(MICROMAG_SPI, bytes, NULL, 2);
	
	return (int16_t)((bytes[0] << 8) | bytes[1]);
}

static void micromag_reset() {
	micromag_set_reset_output(true);
	micromag_set_reset_output(false);
}

static bool micromag_get_drdy_input() {
	return gpio_input(GPIO_PORT_B, 8);
}

static void micromag_set_reset_output(bool reset) {
	gpio_output(GPIO_PORT_B, 5, reset);
}
