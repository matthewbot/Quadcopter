#ifndef STMOS_UTIL_MUTEX_H
#define STMOS_UTIL_MUTEX_H

extern "C" {
	#include <stmos/kernel/mutex.h>
}
#include <stmos/util/NonCopyable.h>

namespace stmos {
	class Mutex : NonCopyable {
		public:
			inline Mutex() { mutex_init(&mutex); }
			
			inline void wait() { mutex_wait(&mutex); }
			inline void release() { mutex_release(&mutex); }
			
		private:
			kernel_mutex mutex;
	};
	
	class Lock : NonCopyable {
		public:
			inline Lock(Mutex &mutex) : mutex(mutex) { mutex.wait(); }
			inline ~Lock() { mutex.release(); }
			
		private:
			Mutex &mutex;
	};	
}

#endif
