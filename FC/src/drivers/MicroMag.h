#ifndef FC_DRIVERS_MICROMAG_H
#define FC_DRIVERS_MICROMAG_H

#include <stmos/peripherals/SPI.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <stdint.h>

namespace FC {
	class MicroMag {
		public:
			__attribute__((packed)) enum Axis {
				AXIS_X,
				AXIS_Y,
				AXIS_Z
			};
			
			__attribute__((packed)) enum Period {
				PERIOD_32,
				PERIOD_64,
				PERIOD_128,
				PERIOD_256,
				PERIOD_512,
				PERIOD_1024,
				PERIOD_2048,
				PERIOD_4096	
			};
			
			typedef int16_t Result;
			
			__attribute__((packed)) struct Scan {
				Result x, y, z;
			};
			
			MicroMag(int spinum, stmos::IOPin::PortPin reset, stmos::IOPin::PortPin drdy);
			
			Result sample(Axis axis, Period period=PERIOD_4096);
			Scan scan(Period period=PERIOD_4096);
			
		private:
			stmos::SPI spi;
			stmos::IOPin reset_pin;
			stmos::IOPinWait drdy_pin;
	};
}

#endif
