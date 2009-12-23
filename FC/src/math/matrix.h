#ifndef FC_MATH_MATRIX_H
#define FC_MATH_MATRIX_H

#include <stmos/peripherals/USART.h>

namespace FC {
	namespace mat {
		void set_identity(float *mat_out, int m);

		void add(const float *mat_a, const float *mat_b, int m, int n, float *mat_out);
		void sub(const float *mat_a, const float *mat_b, int m, int n, float *mat_out);
		void scale(const float *mat_a, float scalar, int m, int n, float *mat_out);

		void transpose(const float *mat, int m, int n, float *mat_out);

		void invert(float *mat, int m);
		void invert2(float *mat);

		// produces a matrix equivalent to a cross product by the specified vector
		void cross_prod_vec(const float *vec, float *mat_out);

		// m is rows of a 
		// n is cols of a
		// n is the rows of b (otherwise can't multiply)
		// p is cols of b
		void multiply(const float *mat_a, const float *mat_b, int m, int n, int p, float *mat_out);
		void multiply_3x3(const float *mat_a, const float *mat_b, float *mat_out);

		void print(const float *mat, int m, int n, stmos::USART &out);
	}
}

#endif
