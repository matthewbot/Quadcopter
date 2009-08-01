#include "math/matrix.h"
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
}


