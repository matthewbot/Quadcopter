#include "trig.h"
#include "vector.h"
#include "matrix.h"
#include "Quaternion.h"
#include <math.h>
#include <stddef.h>
#include <float.h>

namespace FC {
	float angle_between_vec3(const float *veca, const float *vecb, bool skipnormal, float *vec_out) {
		static float veca_normal[3], vecb_normal[3];
	
		if (!skipnormal) {
			vec::normalize(veca, 3, veca_normal);
			vec::normalize(vecb, 3, vecb_normal);
			veca = veca_normal;
			vecb = vecb_normal; 	
		}
	
		float angle = acosf(vec::dotprod(veca, vecb, 3));
	
		if (vec_out != NULL) {
			vec::crossprod(veca, vecb, vec_out);
			vec::normalize(vec_out, 3, vec_out);
		}
	
		return angle;
	}

	void axis_angle_to_mat(const float *axis, float angle, float *mat_out) {
		static float temp_mat[3*3];
		static float axis_mat[3*3];
	
		// R = I + s*~axis + t*(~axis)^2
	
		mat::cross_prod_vec(axis, axis_mat);
	
		mat::set_identity(mat_out, 3); 
	
		mat::scale(axis_mat, sinf(angle), 3, 3, temp_mat);
		mat::add(mat_out, temp_mat, 3, 3, mat_out);
	
		mat::multiply(axis_mat, axis_mat, 3, 3, 3, temp_mat);
		mat::scale(temp_mat, 1 - cosf(angle), 3, 3, temp_mat);
		mat::add(mat_out, temp_mat, 3, 3, mat_out);
	}

	void axis_angle_to_quat(const float *vec, float angle, Quaternion &quat) {
		angle /= 2;
		const float sf = sinf(angle);
	
		quat.w = cosf(angle);
		quat.x = vec[0] * sf;
		quat.y = vec[1] * sf;
		quat.z = vec[2] * sf;
	}

	void axis_angle_to_Euler(const float *vec, float angle, Euler &euler) {
		const float sinangle = sinf(angle);
		const float cosangle1m = 1 - cosf(angle);
	
		const float x = vec[0];
		const float y = vec[1];
		const float z = vec[2];

		const float testval = x*y*cosangle1m + z*sinangle;
		if (fabs(testval) > 0.99999) {
			euler.yaw = 2*atan2f(x*sinf(angle/2), cosf(angle/2));
			euler.pitch = M_PI/2;
			euler.roll = 0;
		
			if (testval < 0) {
				euler.yaw = -euler.yaw;
				euler.pitch = -euler.pitch;
			}
		
			return;
		}

		euler.yaw = atan2f(y*sinangle - x*z*cosangle1m, 1 - (y*y + z*z)*cosangle1m);
		euler.pitch = asinf(x*y*cosangle1m + z*sinangle);
		euler.roll = atan2(x*sinangle - y*z*cosangle1m, 1 - (x*x + z*z)*cosangle1m);
	}


	void Quaternion_to_mat(const Quaternion &quat, float *mat_out) {
		const float w = quat.w;
		const float x = quat.x;
		const float y = quat.y;
		const float z = quat.z;
	
		*mat_out++ = 1 - 2*y*y - 2*z*z;
		*mat_out++ =     2*x*y + 2*w*z;
		*mat_out++ =     2*x*z - 2*w*y;
	
		*mat_out++ =     2*x*y - 2*w*z;
		*mat_out++ = 1 - 2*x*x - 2*z*z;
		*mat_out++ =     2*y*z - 2*w*x;
	
		*mat_out++ =     2*x*z + 2*w*y;
		*mat_out++ =     2*y*z - 2*w*x;
		*mat_out++ = 1 - 2*x*x - 2*y*y;
	}

	void Quaternion_to_Euler(const Quaternion &quat, Euler &euler) {
		const float qw = quat.w;
		const float qx = quat.x;
		const float qy = quat.y;
		const float qz = quat.z;
	
		const float pitchval = qx*qy + qz*qw;
	
		if (fabs(pitchval) > 0.49999) {
			euler.yaw = 2*atan2f(qx, qw);
			euler.pitch = M_PI/2;
			euler.roll = 0;
		
			if (pitchval < 0) {
				euler.yaw = -euler.yaw;
				euler.pitch = -euler.pitch;
			}
		
			return;
		} 

		euler.yaw = atan2f(2*(qy*qw - qx*qz), 1 - 2*(qy*qy - qz*qz));
		euler.pitch = asinf(2*pitchval);
		euler.roll = atan2f(2*(qx*qw - qy*qz), 1 - 2*(qz*qz - qz*qz));
	}

	void Euler_to_Quaternion(const Euler &euler, Quaternion *quat_out) {
		const float c1 = cosf(euler.yaw/2);
		const float c2 = cosf(euler.pitch/2);
		const float c3 = cosf(euler.roll/2);
		const float s1 = sinf(euler.yaw/2);
		const float s2 = sinf(euler.pitch/2);
		const float s3 = sinf(euler.roll/2);	
	
		quat_out->w = c1*c2*c3 - s1*s2*s3;
		quat_out->x = s1*s2*c3 + c1*c2*s3;
		quat_out->y = s1*c2*c3 + c1*s2*s3;
		quat_out->z = c1*s2*c3 - s1*c2*s3;
	}

	//http://www.euclideanspace.com/maths/geometry/rotations/conversions/EulerToMatrix/index.htm
	void Euler_to_mat(const Euler &euler, float *mat_out) {
		const float ch = cosf(euler.yaw);
		const float sh = sinf(euler.yaw);
		const float ca = cosf(euler.pitch);
		const float sa = sinf(euler.pitch);
		const float cb = cosf(euler.roll);
		const float sb = sinf(euler.roll);
	
		*mat_out++ = ch*ca;
		*mat_out++ = sh*sb - ch*sa*cb;
		*mat_out++ = ch*sa*sb + sh*cb;
		*mat_out++ = sa;
		*mat_out++ = ca*cb;
		*mat_out++ = -ca*sb;
		*mat_out++ = -sh*ca;
		*mat_out++ = sh*sa*cb + ch*sb;
		*mat_out++ = -sh*sa*sb + ch*cb;
	}
}
