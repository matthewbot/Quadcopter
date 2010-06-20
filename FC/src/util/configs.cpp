#include "configs.h"

using namespace FC;

const AnalogSampler::Channels FC::configs::analogchannels = { {
	6, 5, 8, 7, 9, 11
}};

const AnalogSensors::Config FC::configs::analogconfig = {
	{
		-4.45E-4, 4.45E-4, -2.5E-4,
		-1.50E-4, -1.56E-4, 1.54E-4
	},
	{
		31960, 32630, 31715
	},
	{ .5, .5, .5, .2, .2, .2 }
};

const IMU::Config FC::configs::imu = {
	{ // roll_pitch_kalman
		{ // Q
			1E-10, 8E-8, 0,
			8E-8, 6.4E-5, 0,
			0, 0, 2.5E-7,
		}, { // R
			6.7716e-4, -4.8451e-4,
			-4.8451e-4, 3.9255e-3,
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
	0 // pitch_offset
};

const MotorsController::Config FC::configs::control = {
	{
		0.13, 0.03, -0.032,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
	    0.13, 0.03, -0.032,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
		0.02, 0, 0,
		.1,
		ESCTimer::UPDATETIME / 1000.0
	}
};
