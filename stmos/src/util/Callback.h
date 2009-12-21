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
	
	class CallbackWrapper : public Callback {
		public:
			typedef void (*Func)();
		
			CallbackWrapper(Func func) : func(func) { }
			virtual void call() { func(); }
			
		private:
			Func func;
	};
}

#endif
