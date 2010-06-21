#ifndef FC_DRIVERS_ANALOGSAMPLER_H
#define FC_DRIVERS_ANALOGSAMPLER_H

#include <FC/math/LowpassFilter.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/DMA.h>
#include <stmos/util/Task.h>
#include <stmos/util/NonCopyable.h>

namespace FC {
	class AnalogSampler : stmos::NonCopyable {
		public:
			static const int SENSOR_COUNT = 6;
		
			union Channels {
				struct {
					stmos::ADC::Channel roll_gyro, pitch_gyro, yaw_gyro;
					stmos::ADC::Channel x_accel, y_accel, z_accel;
				};
	
				stmos::ADC::Channel array[SENSOR_COUNT];
			};
		
			struct Samples {
				struct {
					stmos::ADC::Sample roll, pitch, yaw;
					stmos::ADC::Sample x, y, z;
				};
				
				stmos::ADC::Sample array[SENSOR_COUNT];
			};
		
			AnalogSampler(stmos::ADC &adc, const Channels &channels);
	
			Samples getSamples();
			Samples getAveragedSamples(int samples, int tdelta);
			
		private:		
			stmos::ADC &adc;
			stmos::DMAWait adc_dma;
	};
}

#endif
