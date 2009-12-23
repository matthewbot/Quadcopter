#include "IMU.h"
#include <cmath>

using namespace FC;
using namespace stmos;

IMU::IMU(AnalogSensors &sensors, const Config &config)
: sensors(sensors),
  rollkalman(config.roll_pitch_config),
  pitchkalman(config.roll_pitch_config),
  updatetask("imu", Task::PRIORITY_HIGH, *this, 2048),
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
	stop();
	State ret = { rollkalman.getState().angle, pitchkalman.getState().angle };
	start();
	
	return ret;	
}

extern "C" {
#include <stmos/crt/debug.h>
}

void IMU::call() {
	while (true) {
		unsigned long starttime = Task::getCurrentTick();
		AnalogSensors::Readings readings = sensors.getReadings();
	
		{
			Kalman::Measurement roll = {{atan2f(readings.z_accel, readings.x_accel)+(float)M_PI/2, readings.roll_gyro}};
			rollkalman.step(roll);
		}
	
		{
			Kalman::Measurement pitch = {{atan2f(readings.z_accel, readings.y_accel)+(float)M_PI/2, readings.pitch_gyro}};
			pitchkalman.step(pitch);
		}
		
		int time = 5 - (Task::getCurrentTick() - starttime);
		if (time > 0) {
			Task::sleep(time);
		} else {
			debug_print("x");
		}
	
		cyclecount++;
	}
}
