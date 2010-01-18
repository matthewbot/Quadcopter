#ifndef FC_CONTROL_MOTORS_H
#define FC_CONTROL_MOTORS_H

#include <FC/drivers/ESC.h>
#include <stmos/peripherals/Timer.h>

namespace FC {
	class Motors {
		public:
			Motors(stmos::Timer &tim);
			
			void arm();
			
			void setThrottle(float throttle, float rollcorrection, float pitchcorrection, float yawcorrection);
			void off();
			
			inline float getNorthThrottle() { return north.getThrottle(); }
			inline float getEastThrottle() { return east.getThrottle(); }
			inline float getSouthThrottle() { return south.getThrottle(); }
			inline float getWestThrottle() { return west.getThrottle(); }
		private:
			ESC north, east, south, west;
	};

};

#endif
