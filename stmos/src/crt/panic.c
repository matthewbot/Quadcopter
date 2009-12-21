#include "panic.h"
#include <stmos/crt/nvic.h>
#include <stmos/crt/debug.h>

__attribute__((noreturn))
void panic(const char *msg) {
	nvic_disable_interrupts();
	debug_init();
	
	while (true) {
		debug_print(msg);
		
		debug_flashled(1000000);
		debug_delay(10000000);
	}
}
