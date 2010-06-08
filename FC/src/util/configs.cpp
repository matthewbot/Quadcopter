#include "configs.h"

using namespace FC;

const AnalogSensors::Config FC::configs::analog = {
	{ {6, 5, 8, 7, 9, 11} }, // channels
	{ { // calibrations
		{ // gyros
			{ 32768, -7.407E-4 }, // roll
			{ 32768, 7.407E-4 }, // pitch
			{ 32768, -2.5E-4 } // yaw
		}, { // accels
			{ 32768, -1.4815E-4 }, // x-axis
			{ 32768, -1.4815E-4 }, // y-axis
			{ 32768, 1.4815E-4 }  // z-axis
		}
	} },
	{ .3, .3, .3, 0.01, 0.01, 0.01 } // alphas
};

const IMU::Config FC::configs::imu = {
	{ // roll_pitch_kalman
		{ // Q
			1E-7, 0, 0,
			0, 1.5E-5, 0,
			0, 0, 1E-9
		}, { // R
			1.9725E-5, -5.0045E-6,
			-5.0045E-6, 2.2327E-3,
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
	0.08, // roll_offset
	0 // pitch_offset
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
