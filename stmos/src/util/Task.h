#ifndef STMOS_UTIL_TASK_H
#define STMOS_UTIL_TASK_H

#include <stmos/util/Callback.h>
#include <stmos/util/NonCopyable.h>
#include <stdint.h>
#include <cstddef>

extern "C" struct kernel_task;

namespace stmos {
	class Task : public NonCopyable {
		public:
			Task(const char *name, uint8_t pri, Callback &callback, size_t stacksize=1024);
			~Task();
			
			void suspend();
			void resume();
			void stop();
			
			static void sleep(long msecs);
			Task *getCurrentTask();
		private:
			Task();
			struct kernel_task *ktask;
			
			void wakeup();
			
			static Task maintask;
	};
}


#endif
