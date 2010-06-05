#include "IMU.h"
#include <FC/math/trig.h>
#include <cmath>

using namespace FC;
using namespace stmos;
using namespace std;

IMU::IMU(AnalogSensors &sensors, const Config &config)
: config(config), sensors(sensors),
  rollkalman(config.roll_pitch_config),
  pitchkalman(config.roll_pitch_config),
  yawstate(0), yawstatevel(0),
  updatetask("imu", Task::PRIORITY_HIGH, *this, 1024),
  cyclecount(0) { }
  
void IMU::start() {
	updatetask.resume();
}

void IMU::stop() {
	updatetask.suspend();
}

bool IMU::ready() {
	return cyclecount > 200;
}

IMU::State IMU::getState() {
	State ret = { rollkalman.getState().angle, pitchkalman.getState().angle, yawstate };
	
	return ret;	
}

IMU::State IMU::getVelocityState() {
	State ret = { 
		rollkalman.getState().vel, 
		pitchkalman.getState().vel, 
		yawstatevel };
	
	return ret;	
}

static inline float sqr(float f) { return f*f; }

void IMU::call() {
	while (true) {
		unsigned long starttime = Task::getCurrentTick();
		AnalogSensors::Readings readings = sensors.getReadings();
		
		float g = sqrtf(sqr(readings.x_accel) + sqr(readings.y_accel) + sqr(readings.z_accel));
		float apitch = -asinf(readings.y_accel / g);
		float aroll = -asinf(readings.x_accel / (cosf(apitch)*g));

		apitch += config.pitch_offset;
		aroll += config.roll_offset;

		if (isnan(apitch) || isnan(aroll))
			apitch = aroll = 0;

		rollkalman.step((Kalman::Measurement) {{aroll, readings.roll_gyro}});
		pitchkalman.step((Kalman::Measurement) {{apitch, readings.pitch_gyro}});
		
		yawstatevel = readings.yaw_gyro;
		yawstate += yawstatevel * 0.005;
		
		int time = 5 - (Task::getCurrentTick() - starttime);
		if (time > 0)
			Task::sleep(time);
	
		cyclecount++;
	}
}
