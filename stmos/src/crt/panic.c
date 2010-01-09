#include "panic.h"
#include <stmos/crt/nvic.h>
#include <stmos/crt/debug.h>
#include <stmos/kernel/task.h>
#include <stmos/kernel/sched.h>
#include <string.h>

__attribute__((noreturn))
void panic(const char *msg) {
	nvic_disable_interrupts();
	debug_init();
	
	struct kernel_task *curtask = sched_get_current_task();
	
	static char name[10];
	memcpy(name, curtask->name, sizeof(name));
	name[sizeof(name)-1] = '\0';
	
	while (true) {
		debug_print("Panic: ");
		debug_print(msg);
		debug_print("\nTask: ");
		debug_printhex((uint32_t)curtask);
		debug_print(" ");
		debug_print(name);
		debug_print("\n");
		
		debug_flashled(1000000);
		debug_delay(10000000);
	}
}
