#include "configs.h"

using namespace FC;

const AnalogSensors::Channels FC::configs::chans = { {6, 5, 8, 7, 9, 11} };
const AnalogSensors::Calibrations FC::configs::analog = { {
	{ { 0, -2206 }, // roll
	  { 0, 2206 }, // pitch
	  { 0, -4000 } }, // yaw
	{ { 32000, -6750 }, // x-axis
	  { 32000, -6750 }, // y-axis
	  { 32000, 6750 } } // z-axis
} };

const IMU::Config FC::configs::imu = {
	{ // roll_pitch_kalman
		{ // Q
			1E-10, 1E-7, 1E-9,
			1E-7, 1E-4, 1E-6,
			1E-9, 1E-6, 1E-8
		}, { // R
			.001, 0,
			0,  .001
		}
	}, { // yaw_kalman
		{ // Q
			6.25E-12, 1.25E-9, 2.5E-11,
			1.25E-9, 2.5E-7, 5E-9,
			2.5E-11, 5E-9,1E-10
		}, { // R
			.1, 0,
			0, .0001
		}
	}
};

const MotorsController::Config FC::configs::control = {
	{
	    0.03, 0.01, 0.025,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
		.03, 0, .02,
		.1,
		ESCTimer::UPDATETIME / 1000.0
	}
};
