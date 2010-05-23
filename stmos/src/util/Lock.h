#ifndef STMOS_UTIL_LOCK_H
#define STMOS_UTIL_LOCK_H

#include <stmos/util/Mutex.h>
#include <stmos/util/Notifier.h>

namespace stmos {
	class Lock : NonCopyable {
		public:
			inline Lock(Mutex &mutex) : mutex(mutex) { mutex.wait(); }
			inline ~Lock() { mutex.release(); }
			
		private:
			Mutex &mutex;
	};
}	
#endif
