#ifndef STMOS_UTIL_NOTIFIER_H
#define STMOS_UTIL_NOTIFIER_H

extern "C" {
	#include <stmos/kernel/notifier.h>
}
#include <stmos/util/Mutex.h>
#include <stmos/util/CriticalSection.h>
#include <stmos/util/NonCopyable.h>

namespace stmos {
	class Notifier : NonCopyable {
		public:
			inline Notifier() { notifier_init(&notifier); }
			
			inline void wait() { notifier_wait(&notifier); }
			inline void waitRelease(Mutex &mutex) { notifier_wait_release(&notifier, &mutex.mutex); }
			inline void waitLeave(CriticalSection &crit) { notifier_wait_leave(&notifier, crit.irq); }
			inline void waitLeaveRelease(CriticalSection &crit, Mutex &mutex) {
				notifier_wait_leave_release(&notifier, crit.irq, &mutex.mutex);
			}
			inline void notify() { notifier_notify(&notifier); }
			inline void notifyAll() { notifier_notify_all(&notifier); }
			
		private:
			kernel_notifier notifier;
	};

}

#endif
