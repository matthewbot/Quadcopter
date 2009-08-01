#include "math/matrix.h"
#include "drivers/analog.h"
#include "drivers/time.h"
#include "system.h"
#include <stdio.h>

int main(int argc, char **argv) {
	system_init();

	static float matrixa[] = {
		1, 3, 5,
		2, 4, 6,
		3, 5, 9
	};	
	
	static float matrix_out[3*3];
	
	matrix_multiply(matrixa, matrixa, 3, 3, 3, matrix_out);
	matrix_print(matrix_out, 3, 3);
	
	for (;;) {
		time_sleep(500);
		struct analog_readings readings = analog_get_readings();
		
		printf("RL %4u PT %4u YW %4u X %4u Y %4u Z %4u\n",
			readings.roll_rate, readings.pitch_rate, readings.yaw_rate, readings.x_accel, readings.y_accel, readings.z_accel);
	}
}

