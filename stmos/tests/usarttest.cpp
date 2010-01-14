#include <stmos/peripherals/USART.h>

using namespace stmos;

USART out(1, 115200);

int main(int argc, char **argv) {	
	while (true) {
		out.print("Enter some text: ");
		char buf[20];
		out.getline(buf, sizeof(buf));
		out.printf("You typed: %s\n", buf);
	}
}

