#ifndef FC_CONTROL_ANALOGSENSORS_H
#define FC_CONTROL_ANALOGSENSORS_H

#include <FC/drivers/AnalogSampler.h>
#include <FC/math/LowpassFilter.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/util/Task.h>
#include <stmos/util/Callback.h>

namespace FC {
	class AnalogSensors : stmos::Callback {
		public:
			union Readings {
				struct {
					float roll_gyro, pitch_gyro, yaw_gyro;
					float x_accel, y_accel, z_accel;
				};
				
				float array[6];
			};
			
			struct Config {
				float scales[6];
				stmos::ADC::Sample accelcenters[3];
				float alphas[6];
			};
		
			AnalogSensors(AnalogSampler &sampler, const Config &config);
			void centerGyros();
			inline stmos::ADC::Sample getSensorCenter(int i) { return sensorcenters[i]; }
	
			Readings getReadings();
			
			virtual void call();
			
		private:		
			const Config &config;
			AnalogSampler &sampler;
			stmos::ADC::Sample sensorcenters[6];
			LowpassFilter filters[6];
			stmos::Task task;
	};
}

#endif

