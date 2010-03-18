#include "IMU.h"
#include <FC/math/trig.h>
#include <cmath>

using namespace FC;
using namespace stmos;

IMU::IMU(AnalogSensors &sensors, TCCompass &compass, const Config &config)
: sensors(sensors),
  compass(compass),
  rollkalman(config.roll_pitch_config),
  pitchkalman(config.roll_pitch_config),
  yawkalman(config.yaw_config),
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
	State ret = { rollkalman.getState().angle, pitchkalman.getState().angle, yawkalman.getState().angle };
	
	return ret;	
}

IMU::State IMU::getVelocityState() {
	State ret = { rollkalman.getState().vel, pitchkalman.getState().vel, yawkalman.getState().vel };
	
	return ret;	
}

static inline float sqr(float f) { return f*f; }

void IMU::call() {
	while (true) {
		unsigned long starttime = Task::getCurrentTick();
		AnalogSensors::Readings readings = sensors.getReadings();
		
		float aroll = -atan2f(readings.x_accel, sqrtf(readings.y_accel*readings.y_accel + readings.z_accel*readings.z_accel));
		float apitch = -atan2f(readings.y_accel, sqrtf(readings.x_accel*readings.x_accel + readings.z_accel*readings.z_accel));
	
		rollkalman.step((Kalman::Measurement) {{aroll, readings.roll_gyro}});
		pitchkalman.step((Kalman::Measurement) {{apitch, readings.pitch_gyro}});
		
		float heading = compass.calcHeading(rollkalman.getState().angle, pitchkalman.getState().angle);
		Kalman::Measurement yaw = {{ heading, readings.yaw_gyro }};
		yawkalman.step(yaw);
		
		int time = 5 - (Task::getCurrentTick() - starttime);
		if (time > 0)
			Task::sleep(time);
	
		cyclecount++;
	}
}
