#ifndef FC_MATH_TRIG_H
#define FC_MATH_TRIG_H

#include "Quaternion.h"

namespace FC {
	struct Euler {
		float roll, pitch, yaw;
	};

	// returns in axis-angle
	float angle_between_vec3(const float *veca, const float *vecb, bool skipnormal, float *axis_out);

	void axis_angle_to_mat(const float *axis, float angle, float *mat_out);
	void axis_angle_to_quat(const float *vec, float angle, Quaternion &quat);
	void axis_angle_to_Euler(const float *vec, float angle, Euler &Euler);

	void Quaternion_to_mat(const Quaternion &quat, float *mat_out);
	void Quaternion_to_Euler(const Quaternion &quat, Euler &Euler);

	void Euler_to_Quaternion(const Euler &Euler, Quaternion &quat_out);
	void Euler_to_mat(const Euler &Euler, float *mat_out);
}

#endif
