#include <stmos/peripherals/USART.h>

using namespace stmos;

USART out(1, 11520);

int main(int argc, char **argv) {	
	out.print("Hello world!\n");
}

