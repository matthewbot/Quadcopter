#include "nvic.h"
#include "fault.h"
#include "startup.h"
#include "linker.h"
#include <stm32f10x.h>

struct vector_table {
	void *initial_sp;
	nvic_handler handlers[83];
};

__attribute__ ((aligned (128*4)))
static struct vector_table ram_vectors = {
	&__stack_end,
	{
		&reset,
		&fault_handler, // NMI 
		&fault_handler, // Hard fault
		&fault_handler, // MM fault
		&fault_handler, // Bus fault
		&fault_handler, // Usage fault
		0,      // Reserved
		0,
		0,
		0,
		&fault_handler, // SVCall
		0,
		0,
		&fault_handler, // PendSV
		&fault_handler, // SysTick
	}
};

void nvic_set_handler(int irq, nvic_handler handler) {
	ram_vectors.handlers[irq+15] = handler;
}

void nvic_set_priority(int irq, int priority) {
	priority <<= 4; // lower 4 bits aren't implemented
	
	if (irq < 0)
		SCB->SHP[irq+12] = priority;
	else
		NVIC->IP[irq] = priority;
}

void nvic_set_enabled(int irq, bool enabled) {
	__IO uint32_t *nvicarray = enabled ? NVIC->ISER : NVIC->ICER;

	nvicarray[irq >> 5] = 1 << (irq & 0x1F);
}

void nvic_set_basepri(int priority) {
	priority <<= 4;
	asm ("msr basepri, %0" : : "r" (priority));
}

void nvic_clear_basepri() {
	nvic_set_basepri(0);
}

int nvic_get_activeirq() {
	return (SCB->ICSR & SCB_ICSR_VECTACTIVE) - 16;
}

void nvic_pendsv() {
	SCB->ICSR = SCB_ICSR_PENDSVSET; // force context switch through PendSV
}

void nvic_init() {
	SCB->VTOR = (uint32_t)&ram_vectors;
	SCB->CCR = SCB_CCR_DIV_0_TRP;
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA | SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_MEMFAULTENA;
}

