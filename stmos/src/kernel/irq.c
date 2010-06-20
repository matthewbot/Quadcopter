#include "irq.h"
#include "tasklist.h"
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>

#define KERNEL_IRQ_PRI 15

void irq_setup() {
	nvic_set_priority(SVCall_IRQn, KERNEL_IRQ_PRI);
	nvic_set_priority(SysTick_IRQn, KERNEL_IRQ_PRI);
	nvic_set_priority(PendSV_IRQn, KERNEL_IRQ_PRI);
}

static int switchcount;

void irq_disable_switch() {
	nvic_set_basepri(KERNEL_IRQ_PRI); // exclude kernel level interrupts
	switchcount++;
}

void irq_enable_switch() {
	if (--switchcount == 0)
		nvic_clear_basepri();
}

void irq_force_switch() {
	nvic_pendsv();
}

void irq_enter_critical(int irq) {
	irq_disable_switch();
	nvic_set_enabled(irq, false);
}

void irq_leave_critical(int irq) {
	nvic_set_enabled(irq, true);
	irq_enable_switch();
}

void irq_enter() {
	tasklist_irq_began();
}

void irq_leave() {
	tasklist_irq_ended();
}




