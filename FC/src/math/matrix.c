#include "math/matrix.h"
#include "math/vector.h"
#include <stdio.h>

#define M(mat, row, col) mat[(row)*n + col]

void matrix_add(const float *mat_a, const float *mat_b, int m, int n, float *mat_out) {
	vector_add(mat_a, mat_b, m*n, mat_out);
}

void matrix_sub(const float *mat_a, const float *mat_b, int m, int n, float *mat_out) {
	vector_sub(mat_a, mat_b, m*n, mat_out);
}

void matrix_scale(const float *restrict mat_a, float scalar, int m, int n, float *mat_out) {
	vector_scale(mat_a, scalar, m*n, mat_out);
}

void matrix_transpose(const float *restrict mat, int m, int n, float *restrict mat_out) {
	int i, j;
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			M(mat_out, j, i) = *mat++;
		}
	}
}

// algorithm adapted from http://users.erols.com/mdinolfo/matrix.htm
// I just translated the code into C and made it pretty
// One day I want to learn how this works
void matrix_invert(float *restrict mat, int m) {
	const int n = m; // for M() macro to work

	for (int i=1; i < m; i++) 
		mat[i] /= mat[0]; // normalize row 0
		
	for (int i=1; i < m; i++) { 
		for (int j=i; j < m; j++) { // do a column of L
			float sum = 0.0;
			for (int k = 0; k < i; k++)  
				sum += M(mat, j, k) * M(mat, k, i);
			M(mat, j, i) -= sum;
		}
		
		if (i == m-1) 
			continue;
			
		for (int j=i+1; j < m; j++) {  // do a row of U
			float sum = 0.0;
			
			for (int k = 0; k < i; k++)
				sum += M(mat, i, k) * M(mat, k, j);
				
			M(mat, i, j) = (M(mat, i, j) - sum) / M(mat, i, i);
		}
	}
	
	for (int i = 0; i < m; i++) { // invert L
		for (int j = i; j < m; j++) {
			float x = 1.0;
			
			if ( i != j ) {
				x = 0.0;
				for (int k = i; k < j; k++) 
					x -= M(mat, j, k) * M(mat, k, i);
			}
			
			M(mat, j, i) = x / M(mat, j, j);
		}
	}
	
	for (int i = 0; i < m; i++) {  // invert U
		for (int j = i; j < m; j++) {
			if ( i == j ) 
				continue;
				
			float sum = 0.0;
			for (int k = i; k < j; k++)
				sum += M(mat, k, j) * ((i==k) ? 1.0 : M(mat, i, k));
				
			M(mat, i, j) = -sum;
		}
	}
	
	for (int i = 0; i < m; i++) {  // final inversion
		for (int j = 0; j < m; j++) {
			float sum = 0.0;
			
			for (int k = ((i>j)?i:j); k < m; k++)  
				sum += ((j==k) ? 1.0 : M(mat, j, k)) * M(mat, k, i);
				
			M(mat, j, i) = sum;
		}
	}
}

void matrix_multiply(const float *restrict mat_a, const float *restrict mat_b, int m, const int n, const int p, float *restrict mat_out) {
	int i, j, k;
	for (i=0; i<m; i++) {
		for (j=0; j<p; j++) {
			float acc=0;
			
			// premature optimization is the root of all evil!! 
			// ARM must have an instruction making the M() macro very efficient
			// replacing it with a pointer and incrementing it actually decreases performance
			// (probably due to additional pressure on the register allocator)
			for (k=0; k<n; k++) {
				float tmp = M(mat_a, i, k) * M(mat_b, k, j);
				acc += tmp;
			}			
			
			M(mat_out, i, j) = acc;
		}
	}
}

void matrix_print(const float *restrict mat, int m, int n) {
	int i, j;
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			printf("%f ", *mat++);
		}
		printf("\n");
	}	
}

