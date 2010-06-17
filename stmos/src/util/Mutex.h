#ifndef STMOS_UTIL_MUTEX_H
#define STMOS_UTIL_MUTEX_H

extern "C" {
	#include <stmos/kernel/mutex.h>
}
#include <stmos/util/Task.h>
#include <stmos/util/NonCopyable.h>

namespace stmos {
	class Mutex : NonCopyable {
		friend class Notifier;
	
		public:
			inline Mutex() { mutex_init(&mutex); }
			
			inline void wait() { mutex_wait(&mutex); }
			inline void release() { mutex_release(&mutex); }
			inline Task *getOwner() { return (Task *)mutex_get_owner(&mutex)->userdata; }
			
		private:
			kernel_mutex mutex;
	};
}

#endif
