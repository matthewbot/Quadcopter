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
	uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
	uint32_t r0, r1, r2, r3, r12;
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
		debug_print("Task "); debug_print(sched_curtask->name);
		if (!task_checkstack(sched_curtask))
			debug_print(" <<BAD STACKGUARD>>\n");
		else
			debug_print("\n");
		printreg("ICSR ", SCB->ICSR);
		printreg("SHCSR ", SCB->SHCSR);
		printreg("CFSR ", SCB->CFSR);
		printreg("HFSR ", SCB->HFSR);
		printreg("BFAR ", SCB->BFAR);
		printreg("MMFAR ", SCB->MMFAR);
		printreg("IP ", frame->ip);
		printreg("R0 ", frame->r0);
		printreg("R1 ", frame->r1);
		printreg("R2 ", frame->r2);
		printreg("R3 ", frame->r3);
		printreg("R4 ", frame->r4);
		printreg("R5 ", frame->r5);
		printreg("R6 ", frame->r6);
		printreg("R7 ", frame->r7);
		printreg("R8 ", frame->r8);
		printreg("R9 ", frame->r9);
		printreg("R10 ", frame->r10);
		printreg("R11 ", frame->r11);
		printreg("R12 ", frame->r12);
		printreg("LR ", frame->lr);
		printreg("PSR ", frame->psr);
		printreg("SP ", (uint32_t)(frame + 1));
		
		debug_flashled(10000000);
		debug_delay(10000000);
	}		
}
	
__attribute__((naked))
void fault_handler() { 
	register struct frame *frame asm ("r0");
	
	asm volatile(
		"tst lr, 0b0100;" // determine whether the previously active stack was the process stack or main stack pointer
		"ite ne;"
		"mrsne %0, psp;"
		"moveq %0, sp;"
		
		"stmdb %0!, {r4-r11};" // save the other registers 
		: "=r" (frame)
	);
	
	fault_handler_body(frame);
}
