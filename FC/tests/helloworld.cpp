#include <stmos/peripherals/USART.h>

using namespace stmos;

static USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Hello World!\n");
}

