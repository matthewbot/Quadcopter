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

void nvic_register_handler(int id, nvic_handler handler) {
	ram_vectors.handlers[id+15] = handler;
}

void nvic_enable_interrupt(enum IRQn irq) {
	NVIC_EnableIRQ(irq);
}

void nvic_disable_interrupt(enum IRQn irq) {
	NVIC_DisableIRQ(irq);
}

void nvic_set_priority(enum IRQn irq, int priority) {
	NVIC_SetPriority(irq, priority);
}

__attribute__ ((noreturn))
static void fault() {
	puts("Fault\n");
	panel_set_status(PANEL_STATUS_FAULT);
	status_blink_halt(BLINK_COUNT_FAULT);
}

__attribute__ ((noreturn))
static void unhandled() {
	puts("Unhandled IRQ\n");
	panel_set_status(PANEL_STATUS_FAULT);
	status_blink_halt(BLINK_COUNT_UNHANDLED);
}
