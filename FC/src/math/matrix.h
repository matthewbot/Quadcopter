#ifndef FC_MATH_MATRIX_H
#define FC_MATH_MATRIX_H

void matrix_add(const float *mat_a, const float *mat_b, int m, int n, float *mat_out);
void matrix_sub(const float *mat_a, const float *mat_b, int m, int n, float *mat_out);
void matrix_scale(const float *restrict mat_a, float scalar, int m, int n, float *mat_out);

void matrix_transpose(const float *restrict mat, int m, int n, float *restrict mat_out);

void matrix_invert(float *restrict mat, int m);

// m is rows of a 
// n is cols of a
// n is the rows of b (otherwise can't multiply)
// p is cols of b
void matrix_multiply(const float *restrict mat_a, const float *restrict mat_b, int m, int n, int p, float *restrict mat_out);

void matrix_print(const float *restrict mat, int m, int n);

#endif
