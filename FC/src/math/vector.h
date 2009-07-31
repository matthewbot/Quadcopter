#ifndef FC_MATH_VECTOR_H
#define FC_MATH_VECTOR_H

void vector_add(const float *vec_a, const float *vec_b, int n, float *vec_out);
void vector_sub(const float *vec_a, const float *vec_b, int n, float *vec_out); 
float vector_dotprod(const float *vec_a, const float *vec_b, int n);
void vector_crossprod(const float *restrict vec_a, const float *restrict vec_b, float *restrict vec_out); 

float vector_magnitude(const float *restrict vec, int n);
void vector_normalize(const float *restrict vec, int n, float *restrict vec_out); 
void vector_scale(const float *restrict vec, float scalar, int n, float *restrict vec_out); 

void print_vector(const float *restrict vec, int n);

#endif
