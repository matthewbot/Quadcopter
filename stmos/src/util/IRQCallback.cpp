#include "IRQCallback.h"

extern "C" {
	#include <stmos/kernel/irqcallback.h>
}

using namespace stmos;

void IRQCallback::register_irqcallback() {
	if (registered)
		return;
		
	registered = true;	
	irqcallback_new(wrapperfunc, this);
}

void IRQCallback::wrapperfunc(void *data) {
	IRQCallback *callback = (IRQCallback *)data;
	callback->registered = false;
	callback->irqcallback();
}
