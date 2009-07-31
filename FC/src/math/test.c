#include "matrix.h"
#include "vector.h"

int main(int argc, char **argv) {
	static float matrixa[] = {
		1, 3, 5,
		2, 4, 6,
		3, 5, 9
	};	
	
	static const float veca[] = {
		1, 2, 4
	};		
	
	static float vecout[3];
	
	matrix_invert(matrixa, 3);
	matrix_multiply(veca, matrixa, 1, 3, 3, vecout);
	matrix_print(vecout, 1, 3);
}

