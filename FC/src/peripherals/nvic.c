#include "peripherals/nvic.h"
#include "peripherals/gpio.h"
#include "drivers/panel.h"
#include "crt/vectors.h"
#include "crt/status.h"
#include "stm32f10x.h"
#include <stdio.h>

struct vector_table {
	void *initial_sp;
	nvic_handler handlers[83];
};

__attribute__ ((aligned (128*4)))
static struct vector_table ram_vectors;

static void fault();
static void unhandled();

void nvic_init() {
	int i;
	
	ram_vectors.initial_sp = &__main_stack_top;
	ram_vectors.handlers[0] = &reset;
	
	for (i=1;i<=6;i++) {
		ram_vectors.handlers[i] = &fault;
	}
	
	ram_vectors.handlers[11] = &unhandled;
	ram_vectors.handlers[12] = &unhandled;
	
	for (i=14;i<83;i++) {
		ram_vectors.handlers[i] = &unhandled;
	}
	
	SCB->VTOR = (uint32_t)&ram_vectors;
	SCB->CCR = SCB_CCR_STKALIGN |
	           SCB_CCR_DIV_0_TRP |
	           SCB_CCR_UNALIGN_TRP;
	__enable_irq();
}

void nvic_register_handler(enum IRQn irq, nvic_handler handler, bool enable) {
	ram_vectors.handlers[irq+15] = handler;
	
	if (enable)
		nvic_enable_interrupt(irq);
}

void nvic_enable_interrupt(enum IRQn irq) {
	NVIC->ISER[irq >> 5] = 1 << (irq & 0x1F);
}

void nvic_disable_interrupt(enum IRQn irq) {
	NVIC->ICER[irq >> 5] = 1 << (irq & 0x1F);
}

void nvic_set_priority(enum IRQn irq, int priority) {
	priority <<= NVIC_PRIORITY_DEAD_BITS; // the hardware doesn't actually use the lower bits of the priority, 
	// so we pretend they don't exit

	if (irq < 0) 
		SCB->SHP[irq + 12] = priority; // set priority for system interrupts 
	else 
		NVIC->IP[irq] = priority; // set priority for device interrupts
}

// begins every handler
static int beginhandler(const char *msg) {
	panel_set_status(PANEL_STATUS_FAULT);

	int irq = (int)(SCB->ICSR & SCB_ICSR_VECTACTIVE) - 15;
	printf("%s IRQ %d\n", msg, irq);
	
	return irq;
}

__attribute__ ((noreturn))
static void fault() {
	uint32_t *sp;
	asm volatile ("mov %0, %%sp" : "=r" (sp)); // copy the stack pointer into our sp variable

	int irq = beginhandler("Fault");
	
	printf("PC 0x%08X\n", (unsigned int)sp[6]);
	printf("CFSR %u\n", (unsigned int)SCB->CFSR);
	
	if (irq == MemoryManagement_IRQn)
		printf("MMFAR 0x%08X\n", (unsigned int)SCB->MMFAR);
	else if (irq == BusFault_IRQn)
		printf("BFAR 0x%08X\n", (unsigned int)SCB->BFAR);
	
	status_blink_halt(BLINK_COUNT_FAULT);
}

__attribute__ ((noreturn))
static void unhandled() {
	beginhandler("Unhandled");
	
	status_blink_halt(BLINK_COUNT_UNHANDLED);
}
