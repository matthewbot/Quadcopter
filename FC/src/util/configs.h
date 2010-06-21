#ifndef FC_UTIL_CONFIGS_H
#define FC_UTIL_CONFIGS_H

#include <FC/control/IMU.h>
#include <FC/control/AnalogSensors.h>
#include <FC/control/MotorsController.h>
#include <FC/drivers/AnalogSampler.h>

namespace FC {
	namespace configs {
		extern const AnalogSampler::Channels analogchannels;
		extern const AnalogSensors::Config analogconfig;
		extern const IMU::Config imu;
		extern const MotorsController::Config control;
	}
}

#endif
