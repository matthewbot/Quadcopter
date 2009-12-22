#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stm32f10x.h>

using namespace stmos;

extern "C" {
#include <stmos/crt/debug.h>
}

USART out(1, 115200);
IOPin button(IOPin::PORT_A, 0, IOPin::INPUT);

struct ButtonCallback : public Callback {
	virtual void call() {
		out.print("Button!\n");
	}
};

ButtonCallback callback;

int main(int argc, char **argv) {	
	out.print("Hello World!\n");
	button.setupEXTI(callback, IOPin::EDGE_RISING);
	out.printf("EXTICR %x RTSR %x\n", AFIO->EXTICR[0], EXTI->RTSR);
}

