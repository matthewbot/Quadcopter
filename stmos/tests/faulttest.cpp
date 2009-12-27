#include <stmos/peripherals/USART.h>

using namespace stmos;

USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Test fault!\n");
	*(uint32_t *)0 = 0xBADBAD00;
}
