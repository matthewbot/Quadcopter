#ifndef FC_CONTROL_TCCOMPASS_H
#define FC_CONTROL_TCCOMPASS_H

#include <FC/drivers/MicroMag.h>
#include <FC/drivers/AnalogSensors.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>

namespace FC {
	class TCCompass : stmos::NonCopyable, stmos::Callback {
		public:
			TCCompass(const AnalogSensors::Channels &chans, const AnalogSensors::AccelerometerCalibration &cal, 
			          stmos::ADC &adc, MicroMag &mmag);
	
			inline float readHeading() { return heading; }
			
		private:
			const AnalogSensors::Channels &chans;
			const AnalogSensors::AccelerometerCalibration &cal;
			stmos::ADC &adc;
			MicroMag &mmag;
			
			float heading;
			
			virtual void call();
	};
};

#endif
