#ifndef FC_MATH_VECTOR_H
#define FC_MATH_VECTOR_H

#include <stmos/peripherals/USART.h>

namespace FC {
	namespace vec {
		void add(const float *vec_a, const float *vec_b, int n, float *vec_out);
		void sub(const float *vec_a, const float *vec_b, int n, float *vec_out); 
		float dotprod(const float *vec_a, const float *vec_b, int n);
		void crossprod(const float *vec_a, const float *vec_b, float *vec_out); 

		float magnitude(const float *vec, int n);
		void normalize(const float *vec, int n, float *vec_out); 
		void scale(const float *vec, float scalar, int n, float *vec_out); 

		void print(const float *vec, int n, stmos::USART &out);
	}
}
#endif
