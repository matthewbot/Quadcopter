#ifndef FC_MATH_TRIG_H
#define FC_MATH_TRIG_H

#include <stdbool.h>

// returns in axis-angle
float trig_angle_between_vec3(const float *restrict veca, const float *restrict vecb, bool skipnormal, float *restrict axis_out);

// converts axis-angle to matrix
void trig_axis_angle_to_mat(const float *restrict axis, float angle, float *restrict mat_out);

#endif
