#ifndef STMOS_UTIL_IRQFUNCTION_H
#define STMOS_UTIL_IRQFUNCTION_H

extern "C" {
	#include <stmos/kernel/irq.h>
}

namespace stmos {
	class IRQFunction {
		public:
			inline IRQFunction() { irq_enter(); }
			inline ~IRQFunction() { irq_leave(); }
	};
}

#endif
