#include "math/matrix.h"
#include "drivers/analog.h"
#include "drivers/time.h"
#include "system.h"
#include <stdio.h>

int main(int argc, char **argv) {
	system_init();

	analog_setup(NULL);	
	for (;;) {
		time_sleep(500);
		struct analog_readings readings = analog_get_readings();
		
		printf("RL %4u PT %4u YW %4u X %4u Y %4u Z %4u\n",
			readings.roll_rate, readings.pitch_rate, readings.yaw_rate, readings.x_accel, readings.y_accel, readings.z_accel);
	}
}

