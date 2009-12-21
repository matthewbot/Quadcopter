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

struct frame {
	uint32_t r0, r1, r2, r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t ip;
	uint32_t psr;
};

__attribute__((noreturn))
static void fault_handler_body(struct frame *frame) {
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
		printreg("R0 ", frame->r0);
		printreg("R1 ", frame->r1);
		printreg("R2 ", frame->r2);
		printreg("R3 ", frame->r3);
		printreg("R12 ", frame->r12);
		printreg("LR ", frame->lr);
		printreg("IP ", frame->ip);
		printreg("PSR ", frame->psr);
		printreg("SP ", (uint32_t)(frame + 1));
		printreg("sched_curtask ", (uint32_t)sched_curtask);
		printreg("sched_curtask sp", (uint32_t)sched_curtask->sp);
		
		debug_flashled(10000000);
		debug_delay(10000000);
	}		
}
	
__attribute__((naked))
void fault_handler() { 
	struct frame *frame;
	
	asm volatile(
		"tst lr, 0b0100;"
		"ite ne;"
		"mrsne %0, psp;"
		"moveq %0, sp;"
		
		: "=r" (frame)
	);
	
	fault_handler_body(frame);
}
