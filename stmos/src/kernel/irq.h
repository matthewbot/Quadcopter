#ifndef STMOS_KERNEL_IRQ_H
#define STMOS_KERNEL_IRQ_H

void irq_setup();

void irq_disable_switch();
void irq_enable_switch();

void irq_force_switch();

#endif
