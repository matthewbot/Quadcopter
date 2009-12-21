#include <stmos/peripherals/USART.h>

using namespace stmos;

extern "C" {
#include <stmos/crt/debug.h>
}

USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Hello World!\n");
}

