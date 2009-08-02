#include "math/matrix.h"
#include "drivers/micromag.h"
#include "drivers/analog.h"
#include "system.h"
#include <stdio.h>

static void printmicromag(struct micromag_scan_results results);
static void printanalog(struct analog_readings readings);

int main(int argc, char **argv) {
	system_init();

	micromag_scan(MICROMAG_PERIOD_1024, printmicromag);
	analog_set_update_handler(printanalog);
}

static void printmicromag(struct micromag_scan_results results) {
	//printf("%d %d %d\n", (int)results.x, (int)results.y, (int)results.z);
}

static void printanalog(struct analog_readings readings) {	
	printf("%u %u %u\n", (unsigned int)readings.roll_rate, (unsigned int)readings.pitch_rate, (unsigned int)readings.yaw_rate);
	//printf("%u %u %u\n", (unsigned int)readings.x_accel, (unsigned int)readings.y_accel, (unsigned int)readings.z_accel);
}
