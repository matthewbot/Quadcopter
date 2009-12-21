#ifndef STMOS_UTIL_IRQ_H
#define STMOS_UTIL_IRQ_H

#define IRQ_PRIORITY_LOW 14
#define IRQ_PRIORITY_MED 8
#define IRQ_PRIORITY_HIGH 1

namespace stmos {
	class Callback {
		public:
			virtual void call()=0;
	};
	
	template <void (*func)()> class CallbackWrapper : public Callback {
		public:
			virtual void call() { func(); }
	};
}

#endif
