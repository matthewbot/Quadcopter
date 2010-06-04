#include "configs.h"

using namespace FC;

const AnalogSensors::Channels FC::configs::chans = { {6, 5, 8, 7, 9, 11} };
const AnalogSensors::Calibrations FC::configs::analog = { {
	{ { 32768, -4.5331E-4 }, // roll
	  { 32768, 4.5331E-4 }, // pitch
	  { 32768, -2.5E-4 } }, // yaw
	{ { 32768, -1.4815E-4 }, // x-axis
	  { 32768, -1.4815E-4 }, // y-axis
	  { 32768, 1.4815E-4 } } // z-axis
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
	},
	0, // roll_offset
	0.075 // pitch_offset
};

const MotorsController::Config FC::configs::control = {
	{
		0.07, 0.1, 0.032,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
	    0.07, 0.1, 0.032,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
		0, 0, 0,
		.1,
		ESCTimer::UPDATETIME / 1000.0
	}
};
