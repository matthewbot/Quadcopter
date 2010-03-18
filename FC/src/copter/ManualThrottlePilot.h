#ifndef FC_COPTER_MANUALTHROTTLEPILOT_H
#define FC_COPTER_MANUALTHROTTLEPILOT_H

#include <FC/copter/Pilot.h>

namespace FC {
	class ManualThrottlePilot : public Pilot {
		public:
			ManualThrottlePilot();
			virtual ~ManualThrottlePilot();
			
			virtual void start(Quadcopter &quad);
			virtual void run(Quadcopter &quad);
			
		private:
			float heading;
	};
}

#endif
