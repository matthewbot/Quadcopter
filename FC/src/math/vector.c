#include "vector.h"
#include <math.h>
#include <stdio.h>

void vector_add(const float *vec_a, const float *vec_b, int n, float *vec_out) {
	while (n--) {
		*vec_out++ = *vec_a++ + *vec_b++;
	}
}

void vector_sub(const float *vec_a, const float *vec_b, int n, float *vec_out) {
	while (n--) {
		*vec_out++ = *vec_a++ - *vec_b++;
	}
}

float vector_dotprod(const float *restrict vec_a, const float *restrict vec_b, int n) {
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

void vector_crossprod(const float *restrict vec_a, const float *restrict vec_b, float *restrict vec_out) {
	static const struct vector_crossprod_pair pairs[] = {
		{2, 3},
		{3, 1},
		{1, 2}
	};
	
	const struct vector_crossprod_pair *curpair = pairs;
	int i;
	
	for (i=0; i<3; i++, curpair++, vec_out++) {
		const unsigned int a = curpair->a;
		const unsigned int b = curpair->b;
		
		*vec_out = vec_a[a] * vec_b[b] - vec_a[b] * vec_b[a];
	}
}

float vector_magnitude(const float *restrict vec, int n) {
	float acc=0;
	while (n--) {
		const float val = *vec++;
		acc += val*val;
	}
	
	return sqrtf(acc);
}

void vector_normalize(const float *vec, int n, float *vec_out) {
	float magnitude = vector_magnitude(vec, n);
	
	while (n--) {
		*vec_out++ /= magnitude;
	}
}

void vector_scale(const float *vec, float scalar, int n, float *vec_out) {
	while (n--) {
		*vec_out++ = *vec++ * scalar;
	}
}

void print_vector(const float *vec, int n) {
	while (n--) {
		printf("%f ", *vec++);
	}
	
	printf("\n");
}
