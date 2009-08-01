#include "matrix.h"
#include "vector.h"

int main(int argc, char **argv) {
	static float matrixa[] = {
		1, 3, 5,
		2, 4, 6,
		3, 5, 9
	};	
	
	static float matrix_out[3*3];
	
	matrix_multiply(matrixa, matrixa, 3, 3, 3, matrix_out);
	matrix_print(matrix_out, 3, 3);
}

