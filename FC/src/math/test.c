#include "matrix.h"
#include "vector.h"
#include "trig.h"
#include <stdio.h>

int main(int argc, char **argv) {
	static const float veca[] = {
		0, 1, 0
	};
	
	static const float vecb[] = {
		1, 0, 0
	};
	
	static float vec_axis[3];
	
	float angle = trig_angle_between_vec3(veca, vecb, false, vec_axis);
	printf("Angle: %f\n", angle);
	//vector_crossprod(veca, vecb, vec_axis);
	printf("Axis: "); vector_print(vec_axis, 3);
}

