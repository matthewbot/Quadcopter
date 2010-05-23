#ifndef STMOS_UTIL_CRITICALSECTION_H
#define STMOS_UTIL_CRITICALSECTION_H

extern "C" {
	#include <stmos/kernel/irq.h>
}
#include <stmos/util/NonCopyable.h>

namespace stmos {
	class CriticalSection : NonCopyable {
		friend class Notifier;
		
		public:
			inline CriticalSection(int irq) : irq(irq) { irq_enter_critical(irq); }
			inline ~CriticalSection() { irq_leave_critical(irq); }
			
			inline void enter() { irq_enter_critical(irq); }
			inline void leave() { irq_leave_critical(irq); }
			
		private:
			const int irq;
	};
}

#endif
