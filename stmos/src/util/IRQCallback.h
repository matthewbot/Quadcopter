#ifndef STMOS_UTIL_IRQCALLBACK_H
#define STMOS_UTIL_IRQCALLBACK_H

namespace stmos {
	class IRQCallback {
		public:
			inline IRQCallback() : registered(false) { }
			inline virtual ~IRQCallback() { }
			virtual void irqcallback() = 0;
			
			void register_irqcallback();
			
		private:
			static void wrapperfunc(void *data);
			bool registered;
	};
}

#endif
