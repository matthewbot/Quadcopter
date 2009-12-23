#ifndef FC_DRIVERS_MICROMAG_H
#define FC_DRIVERS_MICROMAG_H

#include <stmos/peripherals/SPI.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Callback.h>
#include <stdint.h>

namespace FC {
	class MicroMag : stmos::Callback {
		public:
			__attribute__((packed)) enum Axis {
				_AXIS_NONE,
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
			
			void startScan(Period period=PERIOD_4096);
			void startScan(Period period, stmos::Callback &call);
			inline Scan getScan() { return scan_results; }
			inline unsigned long getScanTime() { return scan_tottime; }
			void stopScan();
			
		private:
			virtual void call(); // used for an EXTI callback related to scanning
		
			void query(Axis axis, Period period);
			Result getResult();
		
			stmos::SPI spi;
			stmos::IOPin reset_pin, drdy_pin;
			
			Period scan_period;
			stmos::Callback *scan_callback;
			unsigned long scan_prevtime;
			unsigned long scan_tottime;
			Scan scan_results;
			Axis scan_axis; // 0 = no scan going on
	};
}

#endif
