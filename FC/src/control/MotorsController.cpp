#include "MotorsController.h"

using namespace FC;
using namespace stmos;

MotorsController::MotorsController(const Config &config, IMU &imu, Motors &motors, ESCTimer &esctimer)
: config(config),
  imu(imu),
  motors(motors),
  running(false),
  roll_pid(config.roll_config),
  pitch_pid(config.pitch_config),
  yaw_pid(config.yaw_config),
  throttle(0),
  roll_setpoint(0),
  pitch_setpoint(0),
  yaw_setpoint(0),
  roll_correction(0),
  pitch_correction(0),
  yaw_correction(0) {
	esctimer.setOverflowCallback(*this);  
}
  
void MotorsController::start() {
	running = true;
}

void MotorsController::stop() {
	running = false;
}

void MotorsController::setControlPoints(float throttle, float roll, float pitch, float yaw) {
	this->throttle = throttle;
	roll_setpoint = roll;
	pitch_setpoint = pitch;
	yaw_setpoint = yaw;
}

void MotorsController::clearIntegrals() {
	roll_pid.clearInt();
	pitch_pid.clearInt();
	yaw_pid.clearInt();
}

void MotorsController::call() {
	if (!running)
		return;
		
	if (throttle < 0.1) {
		motors.setThrottle(0, 0, 0, 0);
		return;
	}
		
	IMU::State imustate = imu.getState();
	float rollvel = config.rollpitch_stickfactor * (roll_setpoint - imustate.roll);
	float pitchvel = config.rollpitch_stickfactor * (pitch_setpoint - imustate.pitch);
	float yawvel = config.yaw_stickfactor * (yaw_setpoint - imustate.yaw);
	
	IMU::State imuvelstate = imu.getVelocityState();
	roll_correction = roll_pid.update(rollvel - imuvelstate.roll);
	pitch_correction = pitch_pid.update(pitchvel - imuvelstate.pitch);
	yaw_correction = yaw_pid.update(yawvel - imuvelstate.yaw);
	
	motors.setThrottle(throttle, roll_correction, pitch_correction, yaw_correction);
}

