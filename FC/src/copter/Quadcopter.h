#ifndef FC_COPTER_QUADCOPTER_H
#define FC_COPTER_QUADCOPTER_H

#include "QuadcopterBase.h"

namespace FC {
	class Pilot;

	class Quadcopter : QuadcopterBase {
		public:
			Quadcopter(Pilot *initialpilot);
			~Quadcopter();
			
			void run();
			void setPilot(Pilot *pilot);
			
			inline VexRC &getVexRC() { return vexrc; }
			inline MotorsController &getMotorsController() { return motcontrol; }
			inline IMU &getIMU() { return imu; }
			
		private:
			void start();
			void stop();
		
			Pilot *pilot;
			unsigned long cyclecount;
	};
}

#endif
