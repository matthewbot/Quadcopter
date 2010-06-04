#include "handler.h"
#include "tick.h"
#include "sched.h"
#include "irq.h"
#include "irqcallback.h"
#include <stmos/crt/nvic.h>
#include <stmos/crt/fault.h>
#include <stm32f10x.h>
#include <stddef.h>

__attribute__((naked))
void handler_begin() {
	nvic_set_handler(SVCall_IRQn, handler_svcall);
	nvic_set_handler(SysTick_IRQn, handler_systick);
	nvic_set_handler(PendSV_IRQn, handler_pendsv);

	nvic_svc(); // enter handler mode through svc call
	for (;;) { }
}

__attribute__((naked))
void handler_svcall() {
	nvic_set_handler(SVCall_IRQn, fault_handler);
	sched_setup();
	SysTick->LOAD = 72000 - 1;
	SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT;
	
	asm volatile(
		"ldr sp, =__stack_end;" // reset master stack pointer
		
		"ldr r0, =sched_curtask;" // get the address of the current task pointer
	    "ldr r0, [r0];" // read it to get the current task
	    "ldr r1, [r0, %0];" // read the stack pointer
	    "ldmia r1!, {r4-r11};" // load high registers
	    "msr psp, r1;" // set program stack pointer
	    
	    "mov lr, #0xFFFFFFFD;" // set lr to EXC_RETURN 0b1101 return to thread/psp mode
	    
	    "bx lr;\n" // end ISR (microcode pops the lower registers, pc, etc)
	    :: "i" (offsetof(struct kernel_task, sp))
	);
}

__attribute__((naked))
void handler_pendsv() {
	asm("ldr r0, =sched_curtask;" // get the address of the current task pointer 
		"ldr r0, [r0];" // read it to get the address of the current task
		"mrs r1, psp;" // get program stack pointer
		"stmdb r1!, {r4-r11};" // store high registers to program stack pointer
		"str r1, [r0, %[sp]];" // save program stack pointer in task structure

		"mov r4, lr;" // save EXC_RETURN in r4, which is callee preserved
		"bl irqcallback_run;" // run any pending IRQ callbacks
		"bl sched_run;" // update the current task pointer
		"mov lr, r4;" // restore EXC_RETURN
	    
		"ldr r0, =sched_curtask;" // get the address of the current task pointer 
		"ldr r0, [r0];" // read it to get the current task
		"ldr r1, [r0, %[sp]];" // read the first field to get the stack pointer
	    "ldmia r1!, {r4-r11};" // read the high registers
	    "msr psp, r1;" // set program stack pointer
	    
	    "bx lr;" // end ISR (microcode pops lower regs)
	    :: [sp] "i" (offsetof(struct kernel_task, sp))
	);
}

void handler_systick() { 
	tick_run();
	irq_force_switch();
}
