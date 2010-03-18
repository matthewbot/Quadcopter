#ifndef FC_COPTER_PILOT_H
#define FC_COPTER_PILOT_H

#include <FC/copter/Quadcopter.h>
#include <stmos/util/NonCopyable.h>

namespace FC {
	class Pilot : stmos::NonCopyable {
		public:
			virtual ~Pilot();
			
			virtual void start(Quadcopter &copter) = 0;
			virtual void run(Quadcopter &copter) = 0;
			
			const static float CYCLETIME = .1;
	};
}

#endif
