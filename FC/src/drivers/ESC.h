#ifndef FC_DRIVERS_ESC_H
#define FC_DRIVERS_ESC_H

#include <stmos/util/NonCopyable.h>
#include <stmos/peripherals/Timer.h>

namespace FC {
	class ESC : stmos::NonCopyable {
		public:
			ESC(stmos::Timer &tim, int timerchan);
		
			void arm();
			inline bool isArmed() { return armed; }
			void setThrottle(float throttle);
			float getThrottle();
			inline void off() { setThrottle(0); }
			
			void enterProgramMode();
		private:
			stmos::OutputCompareTimerChannel chan;
			bool armed;
	};
}

#endif
