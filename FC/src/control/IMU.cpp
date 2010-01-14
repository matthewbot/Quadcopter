#include "IMU.h"
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

void IMU::call() {
	while (true) {
		unsigned long starttime = Task::getCurrentTick();
		AnalogSensors::Readings readings = sensors.getReadings();
		float compassheading = compass.readHeading();
	
		Kalman::Measurement roll = {{-atan2f(readings.z_accel, readings.x_accel)-(float)M_PI/2, readings.roll_gyro}};
		rollkalman.step(roll);
	
		Kalman::Measurement pitch = {{-atan2f(readings.z_accel, readings.y_accel)-(float)M_PI/2, readings.pitch_gyro}};
		pitchkalman.step(pitch);
		
		Kalman::Measurement yaw = {{ compassheading, readings.yaw_gyro }};
		yawkalman.step(yaw);
		
		int time = 5 - (Task::getCurrentTick() - starttime);
		if (time > 0)
			Task::sleep(time);
	
		cyclecount++;
	}
}
