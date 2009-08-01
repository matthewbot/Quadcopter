#include "drivers/micromag.h"
#include "drivers/time.h"
#include "peripherals/spi.h"
#include "peripherals/gpio.h"
#include "peripherals/delay.h"
#include "peripherals/nvic.h"
#include "peripherals/exti.h"
#include "peripherals/timer.h"
#include <stdint.h>
#include <stdio.h>

#define MICROMAG_SPI 2
#define MICROMAG_DRDY_PORT GPIO_PORT_B
#define MICROMAG_DRDY_PIN 8
#define MICROMAG_RESET_PORT GPIO_PORT_B
#define MICROMAG_RESET_PIN 5

// holds scan (automatic interrupt driven reading) settings
static volatile enum micromag_period scan_period;
static volatile enum micromag_axis scan_axis; // 0 = no scan, 1-3 = current scan channel
static volatile struct micromag_scan_results scan_results;
static volatile unsigned long scan_prevtime;
static volatile unsigned long scan_tottime;
static volatile micromag_scan_handler scan_handler;

static void micromag_query(enum micromag_axis axis, enum micromag_period period);
static int16_t micromag_read_response();
static void micromag_drdy_handler();
static void micromag_pulse_reset();
static bool micromag_get_drdy_input();
static void micromag_timeout_handler();

void micromag_init() {
	spi_setup(MICROMAG_SPI, SPI_BAUD_DIV_128, false);
	exti_setup(MICROMAG_DRDY_PIN, MICROMAG_DRDY_PORT, EXTI_EDGE_RISING, micromag_drdy_handler);
	timer_add_overflow_callback(2, micromag_timeout_handler);
}

int16_t micromag_sample(enum micromag_axis axis, enum micromag_period period) {
	scan_axis = 0; // shut off any in-progress scan
	
	micromag_query(axis, period); // query the micromag
	while (!micromag_get_drdy_input()) { } // wait for data ready
	return micromag_read_response(); // read its response
}

struct micromag_scan_results micromag_get_scan() {
	return scan_results;
}

void micromag_scan(enum micromag_period period, micromag_scan_handler handler) {
	scan_handler = handler;
	scan_prevtime = time_get();
	scan_period = period;
	micromag_query(MICROMAG_AXIS_X, period);
	scan_axis = MICROMAG_AXIS_X;
}

unsigned long micromag_get_scan_time() {
	return scan_tottime;
}

bool micromag_scan_ok() {
	return time_get() - scan_prevtime < 60;
}

static void micromag_drdy_handler() {
	if (scan_axis == 0)
		return;

	int16_t response = micromag_read_response(); // read the current response
	
	if (scan_axis == MICROMAG_AXIS_X) {
		scan_results.x = response;
	} else if (scan_axis == MICROMAG_AXIS_Y) {
		scan_results.y = response;
	} else {
		scan_results.z = response;
		scan_axis = 0; // reset to 0, so the following bump will make it 1, MICROMAG_X
			
		unsigned curtime = time_get(); // update time
		scan_tottime = curtime - scan_prevtime;
		scan_prevtime = curtime;
		
		if (scan_handler)
			scan_handler(scan_results);
	}
	
	scan_axis++;
		
	micromag_query(scan_axis, scan_period);
}

static void micromag_query(enum micromag_axis axis, enum micromag_period period) {
	micromag_pulse_reset();
	
	uint8_t command_byte = axis | (period << 4);
	spi_send_receive(MICROMAG_SPI, NULL, &command_byte, 1);
}

static int16_t micromag_read_response() {
	uint8_t bytes[2];
	spi_send_receive(MICROMAG_SPI, bytes, NULL, 2);
	
	return (int16_t)((bytes[0] << 8) | bytes[1]);
}

static bool micromag_get_drdy_input() {
	return gpio_input(MICROMAG_DRDY_PORT, MICROMAG_DRDY_PIN);
}

static void micromag_pulse_reset() {
	gpio_output(MICROMAG_RESET_PORT, MICROMAG_RESET_PIN, true);
	gpio_output(MICROMAG_RESET_PORT, MICROMAG_RESET_PIN, false);
}

static void micromag_timeout_handler() {
	if (!micromag_scan_ok()) // if our scan seems to have frozen
		micromag_query(scan_axis, scan_period); // try resending the query
}
