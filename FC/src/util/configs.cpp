#include "configs.h"

using namespace FC;

const AnalogSensors::Channels FC::configs::chans = { {6, 5, 8, 7, 9, 11} };
const AnalogSensors::Calibrations FC::configs::analog = { {
	{ { 33280, -7.615E-4 }, // roll
	  { 31648, 7.615E-4 }, // pitch
	  { 33430, -2.5E4 } }, // yaw
	{ { 32000, -1.4815E-4 }, // x-axis
	  { 32000, -1.4815E-4 }, // y-axis
	  { 32000, 1.4815E-4 } } // z-axis
} };

const IMU::Config FC::configs::imu = {
	{ // roll_pitch_kalman
		{ // Q
			1E-10, 1E-8, 1E-10,
			1E-8, 1E-6, 1E-8,
			1E-10, 1E-8, 1E-10
		}, { // R
			.0005, 0,
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
		0.08, 0.08, 0.028,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
	    0.08, 0.08, 0.028,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
		0, 0, 0,
		.1,
		ESCTimer::UPDATETIME / 1000.0
	}
};
