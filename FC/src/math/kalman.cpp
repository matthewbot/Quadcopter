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
	
	static State x_pred;
	x_pred = x;
	x_pred.angle += x_pred.vel*KALMAN_DT;
	
	static CovarianceMat P_pred;
	mat::multiply_3x3(F, P, tmp);
	mat::multiply_3x3(tmp, Ft, P_pred);
	mat::add(config.Q, P_pred, 3, 3, P_pred);
	
	static Measurement y;
	y.accelangle = z.accelangle - x_pred.angle;
	y.gyrovel = z.gyrovel - (x_pred.vel - x_pred.veloffset);
	
	static float S[2*2];
	mat::multiply(H, P_pred, 2, 3, 3, tmp);
	mat::multiply(tmp, Ht, 2, 3, 2, S);
	mat::add(S, config.R, 2, 2, S);
	
	static float K[3*2];
	mat::multiply(P_pred, Ht, 3, 3, 2, tmp);
	mat::invert2(S);
	mat::multiply(tmp, S, 3, 2, 2, K);
	
	mat::multiply(K, y.vec, 3, 2, 1, tmp);
	vec::add(x_pred.vec, tmp, 3, x.vec);
	
	mat::multiply(K, H, 3, 2, 3, tmp);
	mat::sub(I33, tmp, 3, 3, tmp);
	mat::multiply_3x3(tmp, P_pred, P);
}

