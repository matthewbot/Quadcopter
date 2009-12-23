#ifndef FC_MATH_QUATERNION_H
#define FC_MATH_QUATERNION_H

namespace FC {
	struct Quaternion {
		float w, x, y, z;
		
		float magnitude() const;
		Quaternion operator*(const Quaternion &other) const;
		void normalize();
		
		static const Quaternion identity;
	};
}

#endif
