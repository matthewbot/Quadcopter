#ifndef FC_DRIVERS_BATTERYMONITOR_H
#define FC_DRIVERS_BATTERYMONITOR_H

#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/NonCopyable.h>

namespace FC {
	class BatteryMonitor : stmos::NonCopyable {
		public:
			BatteryMonitor(stmos::ADC &adc, stmos::ADC::Channel chan);
			
			float getVoltage();
			float getCellVoltage();
			
		private:
			stmos::ADC &adc;
			stmos::ADC::Channel chan;
			
			stmos::IOPinConfig pin;
	};
}

#endif
