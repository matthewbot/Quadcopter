#ifndef FC_CONTROL_IMU_H
#define FC_CONTROL_IMU_H

#include <FC/math/Kalman.h>
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
				float roll_offset, pitch_offset;
			};
			
			IMU(AnalogSensors &sensors, const Config &config);
			
			void start();
			void stop();
			bool ready();
			inline float getCPUUsage() { return updatetask.getCPUUsage(); }
			
			State getState();
			State getVelocityState();
			inline float getRoll() { return getState().roll; }
			inline float getPitch() { return getState().pitch; }
			inline float getYaw() { return getState().yaw; }
			
		private:
			const Config &config;
			AnalogSensors &sensors;
			Kalman rollkalman, pitchkalman;
			float yawstate, yawstatevel;
			stmos::Task updatetask;
			
			unsigned volatile long cyclecount;
			
			virtual void call();
	};
}

#endif
