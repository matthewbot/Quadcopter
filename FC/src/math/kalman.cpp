#include "Kalman.h"
#include "matrix.h"
#include "vector.h"
#include <math.h>
#include <stdio.h>

using namespace FC;

const static float F[3*3] = {
	1, KALMAN_DT, 0,
	0, 1, 0,
	0, 0, 1
};

const static float Ft[3*3] = {
	1, 0, 0,
	KALMAN_DT, 1, 0,
	0, 0, 1
};

const static float H[2*3] = {
	1, 0, 0,
	0, 1, -1
};

const static float Ht[3*2] = {
	1, 0,
	0, 1,
	0, -1
};


const static float I33[3*3] = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

Kalman::Kalman(const Config &config) : config(config) {
	x.angle = 0;
	x.vel = x.veloffset = 0;
	mat::set_identity(P, 3);
}

void Kalman::step(const Measurement &z) {
	static float tmp[3*3];
	
	// x_pred. = F*x
	static State x_pred;
	x_pred = x;
	x_pred.angle += x_pred.vel*KALMAN_DT;
	//printf("x_pred.: "); vec::print(x_pred..vec, 3);
	
	// P_pred = F*P*transpose(F) + Q
	static CovarianceMat P_pred;
	mat::multiply_3x3(F, P, tmp);
	mat::multiply_3x3(tmp, Ft, P_pred);
	mat::add(config.Q, P_pred, 3, 3, P_pred);
	//printf("P_pred:\n"); mat::print(P_pred, 3, 3);
	
	// y = z - H*x_pred.
	static Measurement y;
	y.accelangle = z.accelangle - x_pred.angle;
	y.gyrovel = z.gyrovel - (x_pred.vel - x_pred.veloffset);
	//printf("y: "); vec::print(y.vec, 2);
	
	// S = H*P_pred*transpose(H) + R
	static float S[2*2];
	mat::multiply(H, P_pred, 2, 3, 3, tmp);
	mat::multiply(tmp, Ht, 2, 3, 2, S);
	mat::add(S, config.R, 2, 2, S);
	//printf("S:\n"); mat::print(S, 2, 2);
	
	// K = P_pred*transpose(H)*inv(S)
	static float K[3*2];
	mat::multiply(P_pred, Ht, 3, 3, 2, tmp);
	mat::invert2(S);
	mat::multiply(tmp, S, 3, 2, 2, K);
	//printf("K:\n"); mat::print(K, 3, 2);
	
	// x_est = x_pred. + K*y
	mat::multiply(K, y.vec, 3, 2, 1, tmp);
	vec::add(x_pred.vec, tmp, 3, x.vec);
	//printf("x_est:\n"); vec::print(filter->x.vec, 3);
	
	// P_est = (eye(3) - K*H)*P_pred
	mat::multiply(K, H, 3, 2, 3, tmp);
	mat::sub(I33, tmp, 3, 3, tmp);
	mat::multiply_3x3(tmp, P_pred, P);
	//printf("P_est:\n"); mat::print(filter->P, 3, 3);
}

