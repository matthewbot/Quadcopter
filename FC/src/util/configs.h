#ifndef FC_UTIL_CONFIGS_H
#define FC_UTIL_CONFIGS_H

#include <FC/control/IMU.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/control/MotorsController.h>

namespace FC {
	namespace configs {
		extern const AnalogSensors::Channels chans;
		extern const AnalogSensors::Calibrations analog;
		extern const float alphas[];
		extern const IMU::Config imu;
		extern const MotorsController::Config control;
	}
}

#endif
