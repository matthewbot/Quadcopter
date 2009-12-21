#include "fault.h"
#include "debug.h"
#include "nvic.h"
#include <stm32f10x.h>
#include <stdbool.h>
#include <stmos/kernel/task.h>

static void printreg(const char *msg, uint32_t reg) {
	debug_print(msg);
	debug_printhex(reg);
	debug_print("\n");
}

extern struct kernel_task *sched_curtask;

__attribute__((naked)) // don't generate prologue/epilogue
void fault_handler() {
	uint32_t *sp;
	asm volatile ("mov %0, %%sp" : "=r" (sp));

	nvic_disable_interrupts();
	debug_init();
	
	while (true) {
		debug_print("Fault\n");
		printreg("ICSR ", SCB->ICSR);
		printreg("SHCSR ", SCB->SHCSR);
		printreg("CFSR ", SCB->CFSR);
		printreg("HFSR ", SCB->HFSR);
		printreg("BFAR ", SCB->BFAR);
		printreg("MMFAR ", SCB->MMFAR);
		printreg("R0 ", sp[0]);
		printreg("R1 ", sp[1]);
		printreg("R2 ", sp[2]);
		printreg("R3 ", sp[3]);
		printreg("R12 ", sp[4]);
		printreg("LR ", sp[5]);
		printreg("IP ", sp[6]);
		printreg("PSR ", sp[7]);
		printreg("SP ", (uint32_t)&sp[8]);
		printreg("sched_curtask ", (uint32_t)sched_curtask);
		printreg("sched_curtask sp", (uint32_t)sched_curtask->sp);
		
		debug_flashled(10000000);
		debug_delay(10000000);
	}		
}
	
