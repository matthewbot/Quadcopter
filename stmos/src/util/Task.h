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
			static const uint8_t PRIORITY_IDLE = 0xFF;
			static const uint8_t PRIORITY_LOWEST = 0xFE; // FF reserved for idle task
			static const uint8_t PRIORITY_LOW = 0xC0;
			static const uint8_t PRIORITY_MID = 0x80;
			static const uint8_t PRIORITY_HIGH = 0x40;
			static const uint8_t PRIORITY_HIGHEST = 0x00;
		
			Task(const char *name, uint8_t pri, Callback &callback, size_t stacksize=1024);
			~Task();
			
			inline void start() { resume(); }
			void suspend();
			void resume();
			void stop();
					
			static unsigned long getCurrentTick();
			static void sleep(unsigned long msecs);
			static Task *getCurrentTask();
		private:
			Task();
			struct kernel_task *ktask;
			
			void wakeup();
			
			static Task maintask;
	};
}


#endif
