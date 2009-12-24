#ifndef FC_CONTROL_MOTORS_H
#define FC_CONTROL_MOTORS_H

#include <FC/drivers/ESC.h>
#include <stmos/peripherals/Timer.h>

namespace FC {
	class Motors {
		public:
			enum Motor {
				NORTH,
				EAST,
				SOUTH,
				WEST
			};
		
			Motors(int timernum);
			
			void arm(Motor mot);
			void armAll();
			
			void setThrottle(Motor mot, float throttle);
			void setThrottleAll(float throttle);
			
		private:
			stmos::Timer tim;
			ESC north, east, south, west;
			
			ESC &getESC(Motor mot);
	};

};

#endif
