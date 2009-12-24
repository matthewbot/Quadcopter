#include "PID.h"

using namespace FC;

PID::PID(const PID::Config &config)
: config(config),
  prev_error(0),
  int_error(0) { }
  
float PID::update(float error) {
	float derror_dt = (error - prev_error) / config.dt;
	prev_error = error;
	
	int_error += error * config.dt;
	if (int_error > config.i_max)
		int_error = config.i_max;
	else if (int_error < -config.i_max)
		int_error = -config.i_max;
		
	return config.p*error + config.i*int_error + config.d*derror_dt;
}

