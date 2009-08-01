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
		const volatile uint16_t *raw = analog_get_raw();
		
		int i;
		for (i=0;i<6;i++) {
			printf("%d ", (int)raw[i]);
		}
		printf("\n");
	}
}

