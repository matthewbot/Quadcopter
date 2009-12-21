#ifndef STMOS_KERNEL_HANDLER_H
#define STMOS_KERNEL_HANDLER_H

__attribute__((noreturn))
void handler_begin();

void handler_svcall();
void handler_pendsv(); // used to switch contexts
void handler_systick(); // used to keep the system tick count and preempt tasks

#endif
