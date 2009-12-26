#ifndef FC_MISC_PPMTIMER_H
#define FC_MISC_PPMTIMER_H

#include <stmos/peripherals/Timer.h>

namespace FC {
	class PPMTimer : public stmos::Timer {
		public:
			PPMTimer();
	};
}

#endif
