#include <stdio.h>
#include "system.h"
#include "peripherals/usart.h"

const char buf[] = "Hello World!\n";

int main(int argc, char **argv) {
	system_init();
	
	usart_send(1, (const uint8_t *)buf, sizeof(buf));
	
	return 0;
}
