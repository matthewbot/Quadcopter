#include "math/trig.h"
#include "math/vector.h"
#include "math/matrix.h"
#include <math.h>
#include <stddef.h>

float trig_angle_between_vec3(const float *restrict veca, const float *restrict vecb, bool skipnormal, float *restrict vec_out) {
	static float veca_normal[3], vecb_normal[3];
	
	if (!skipnormal) {
		vector_normalize(veca, 3, veca_normal);
		vector_normalize(vecb, 3, vecb_normal);
		veca = veca_normal;
		vecb = vecb_normal; 	
	}
	
	float angle = acosf(vector_dotprod(veca, vecb, 3));
	
	if (vec_out != NULL) {
		vector_crossprod(veca, vecb, vec_out);
		vector_normalize(vec_out, 3, vec_out);
	}
	
	return angle;
}

void trig_axis_angle_to_mat(const float *restrict axis, float angle, float *restrict mat_out) {
	static float temp_mat[3*3];
	static float axis_mat[3*3];
	
	// R = I + s*~axis + t*(~axis)^2
	
	matrix_cross_prod_vec(axis, axis_mat);
	
	matrix_set_identity(3, mat_out); 
	
	matrix_scale(axis_mat, sinf(angle), 3, 3, temp_mat);
	matrix_add(mat_out, temp_mat, 3, 3, mat_out);
	
	matrix_multiply(axis_mat, axis_mat, 3, 3, 3, temp_mat);
	matrix_scale(temp_mat, 1 - cosf(angle), 3, 3, temp_mat);
	matrix_add(mat_out, temp_mat, 3, 3, mat_out);
}

