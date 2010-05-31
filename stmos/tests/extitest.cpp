#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stm32f10x.h>

using namespace stmos;
USART out(1, 115200);
IOPinWait button(IOPin::PORT_A, 0);

int main(int argc, char **argv) {	
	out.print("Hello World!\n");
	while (true) {
		button.wait(IOPinWait::EDGE_FALLING);
		out.print("Pushed button!\n");
		button.wait(IOPinWait::EDGE_RISING);
	}
}

