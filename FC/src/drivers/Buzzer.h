#ifndef FC_DRIVERS_BUZZER_H
#define FC_DRIVERS_BUZZER_H

#include <stmos/util/NonCopyable.h>
#include <stmos/peripherals/IOPin.h>

namespace FC {
	class Buzzer : stmos::NonCopyable {
		public:
			Buzzer(bool autobuzz=true);
			
			void buzz(long time);
			
			inline void on() { pin = false; }
			inline void off() { pin = true; }
			
		private:
			stmos::IOPin pin;
	};
}

#endif
