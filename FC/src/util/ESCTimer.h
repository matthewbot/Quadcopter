#ifndef FC_MISC_ESCTIMER_H
#define FC_MISC_ESCTIMER_H

#include <stmos/peripherals/Timer.h>

namespace FC {
	class ESCTimer : public stmos::Timer {
		public:
			ESCTimer();
	};
}

#endif
