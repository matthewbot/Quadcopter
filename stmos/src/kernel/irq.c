#include "irq.h"
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

