#ifndef FC_CONTROL_MOTORS_H
#define FC_CONTROL_MOTORS_H

#include <FC/drivers/ESC.h>
#include <stmos/peripherals/Timer.h>

namespace FC {
	class Motors {
		public:
			Motors(int timernum);
			
			void arm();
			
			void setThrottle(float throttle, float rollcorrection, float pitchcorrection, float yawcorrection);
			void off();
			
		private:
			stmos::Timer tim;
			ESC north, east, south, west;
	};

};

#endif
