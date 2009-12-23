#include "matrix.h"
#include "vector.h"

#define M(mat, row, col) mat[(row)*n + col]
#define Mb(mat, row, col) mat[(row)*p + col]

using namespace stmos;

namespace FC {
	namespace mat {
		void set_identity(float *mat_out, int m) {
			const int n = m; // for M macro to work
	
			int i, j;
			for (i=0; i < m; i++) {
				for (j=0; j < n; j++) {
					*mat_out++ = (i == j) ? 1 : 0;
				}
			}
		}

		void add(const float *mat_a, const float *mat_b, int m, int n, float *mat_out) {
			vec::add(mat_a, mat_b, m*n, mat_out);
		}

		void sub(const float *mat_a, const float *mat_b, int m, int n, float *mat_out) {
			vec::sub(mat_a, mat_b, m*n, mat_out);
		}

		void scale(const float *mat_a, float scalar, int m, int n, float *mat_out) {
			vec::scale(mat_a, scalar, m*n, mat_out);
		}

		void transpose(const float *mat, int m, int n, float *mat_out) {
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
		void invert(float *mat, int m) {
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

		// Simpler 2x2 only inverse, Kalman filter only actually uses this one
		void invert2(float *mat) {
			const int n=2;
	
			float scale = 1 / (M(mat, 0, 0)*M(mat, 1, 1) - M(mat, 0, 1)*M(mat, 1, 0));
	
			float tmp = M(mat, 0, 0); // swap upper left and lower right, multiply by scale
			M(mat, 0, 0) = scale*M(mat, 1, 1);
			M(mat, 1, 1) = scale*tmp;
	
			// flip signs of lower left and upper right, multiply by scale
			M(mat, 0, 1) *= -scale;
			M(mat, 1, 0) *= -scale;
		}

		void cross_prod_vec(const float *vec, float *mat_out) {
			const int n=3; // for M macro
	
			int i;
			for (i=0;i<3;i++) {
				M(mat_out, i, i) = 0;
			}
	
			M(mat_out, 1, 0) = vec[2];
			M(mat_out, 0, 1) = -vec[2];
			M(mat_out, 2, 0) = vec[1];
			M(mat_out, 0, 2) = -vec[1];
			M(mat_out, 2, 1) = vec[0];
			M(mat_out, 1, 2) = -vec[0];
		}

		void multiply(const float *mat_a, const float *mat_b, int m, const int n, const int p, float *mat_out) {
			int i, j, k;
	
			for (i=0; i<m; i++) {
				for (j=0; j<p; j++) {
					float acc = 0;
					for (k=0; k<n; k++) {
						acc += M(mat_a, i, k) * Mb(mat_b, k, j);
					}
					*mat_out++ = acc;
				}
			}
		}

		void multiply_3x3(const float *mat_a, const float *mat_b, float *mat_out) {
			const int n=3;
			int i, j;
	
			for (i=0; i<3; i++) {
				for (j=0; j<3; j++) {
					const float *mat_a_ptr = &mat_a[i*3];
					float acc = *mat_a_ptr++ * M(mat_b, 0, j);
					acc += *mat_a_ptr++ * M(mat_b, 1, j);
					acc += *mat_a_ptr++ * M(mat_b, 2, j);

					*mat_out++ = acc;
				}
			}
		}

		void print(const float *mat, int m, int n, USART &out) {
			int i, j;
			for (i=0; i<m; i++) {
				for (j=0; j<n; j++) {
					out.printf("%.4e ", *mat++);
				}
				out.printf("\n");
			}	
		}
	}
}

