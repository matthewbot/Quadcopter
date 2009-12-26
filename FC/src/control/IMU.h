#ifndef FC_CONTROL_IMU_H
#define FC_CONTROL_IMU_H

#include <FC/math/Kalman.h>
#include <FC/control/TCCompass.h>
#include <FC/drivers/AnalogSensors.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>
#include <stmos/util/Task.h>

namespace FC {
	class IMU : stmos::NonCopyable, stmos::Callback {
		public:
			struct State {
				float roll, pitch, yaw;
			};
			
			struct Config {
				Kalman::Config roll_pitch_config;
				Kalman::Config yaw_config;
			};
			
			IMU(AnalogSensors &sensors, TCCompass &compass, const Config &config);
			
			void start();
			void stop();
			bool ready();
			
			State getState();
			
		private:
			AnalogSensors &sensors;
			TCCompass &compass;
			Kalman rollkalman, pitchkalman, yawkalman;
			stmos::Task updatetask;
			
			unsigned volatile long cyclecount;
			
			virtual void call();
	};
}

#endif
