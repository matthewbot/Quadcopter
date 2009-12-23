#ifndef STMOS_PERIPHERALS_DMA_H
#define STMOS_PERIPHERALS_DMA_H

#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>
#include <cstddef>

namespace stmos {
	class DMA : NonCopyable {
		public:
			enum Priority {
				PRIORITY_LOW,
				PRIORITY_MEDIUM,
				PRIORITY_HIGH,
				PRIORITY_VERY_HIGH
			};
			
			enum Direction {
				DIRECTION_MEM_TO_PER,
				DIRECTION_PER_TO_MEM
			};
		
			DMA(int num);
			
			void setup(Direction dir, Priority pri, size_t tsize);
			void setIRQHandler(Callback &call, int pri=IRQ_PRIORITY_LOW);
			void setIRQEnabled(bool enabled);
			inline void enableIRQ() { setIRQEnabled(true); }
			inline void disableIRQ() { setIRQEnabled(false); }
		
			void start(volatile void *mem, volatile void *periph, size_t count, bool circular);
			void suspend();
			void resume();
			
			size_t getRemaining() const;
			inline bool done() const { return getRemaining() == 0; }
			
		private:
			int num;
	};
}

#endif
