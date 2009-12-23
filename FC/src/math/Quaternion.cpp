#include "Quaternion.h"
#include <cmath>

using namespace FC;

const Quaternion Quaternion::identity = { 1, 0, 0, 0 };

float Quaternion::magnitude() const {
	return sqrtf(w*w + x*x + y*y + z*z);
}

void Quaternion::normalize() {
	float mag = magnitude();
	w /= mag;
	x /= mag;
	y /= mag;
	z /= mag;
}

Quaternion Quaternion::operator*(const Quaternion &other) const {
	Quaternion out;
	
	out.w = w*other.w - x*other.x - y*other.y - z*other.z;
	out.x = w*other.x + x*other.w + y*other.z - z*other.y;
	out.y = w*other.y - x*other.z + y*other.w + z*other.x;
	out.z = w*other.z + x*other.y - y*other.x + z*other.w;
	
	return out;
}

