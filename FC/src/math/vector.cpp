#include "vector.h"
#include <cmath>

using namespace stmos;

namespace FC {
	namespace vec {
		void add(const float *vec_a, const float *vec_b, int n, float *vec_out) {
			while (n--) {
				*vec_out++ = *vec_a++ + *vec_b++;
			}
		}

		void sub(const float *vec_a, const float *vec_b, int n, float *vec_out) {
			while (n--) {
				*vec_out++ = *vec_a++ - *vec_b++;
			}
		}

		float dotprod(const float *vec_a, const float *vec_b, int n) {
			float acc=0;
			while (n--) {
				acc += *vec_a++ * *vec_b++;
			}
	
			return acc;
		}

		struct vector_crossprod_pair {
			unsigned int a : 4;
			unsigned int b : 4;
		};

		void crossprod(const float *vec_a, const float *vec_b, float *vec_out) {
			static const struct vector_crossprod_pair pairs[] = {
				{1, 2},
				{2, 0},
				{0, 1}
			};
	
			const struct vector_crossprod_pair *curpair = pairs;
			int i;
	
			for (i=0; i<3; i++, curpair++, vec_out++) {
				const unsigned int a = curpair->a;
				const unsigned int b = curpair->b;
		
				*vec_out = vec_a[a] * vec_b[b] - vec_a[b] * vec_b[a];
			}
		}

		float magnitude(const float *vec, int n) {
			float acc=0;
			while (n--) {
				const float val = *vec++;
				acc += val*val;
			}
	
			return sqrtf(acc);
		}

		void normalize(const float *vec, int n, float *vec_out) {
			scale(vec, 1 / magnitude(vec, n), n, vec_out);
		}

		void scale(const float *vec, float scalar, int n, float *vec_out) {
			while (n--) {
				*vec_out++ = *vec++ * scalar;
			}
		}

		void print(const float *vec, int n, USART &out) {
			while (n--) {
				out.printf("%.4e ", *vec++);
			}
	
			out.printf("\n");
		}
	}
}
