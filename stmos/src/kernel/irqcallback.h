#ifndef STMOS_KERNEL_IRQCALLBACK_H
#define STMOS_KERNEL_IRQCALLBACK_H

// IRQ callbacks allow an IRQ to register a callback to be run just before the next context switch.
// This acts like a cheap one-off task of the highest priority, giving IRQs a way to safely
// manipulate task-only structures like the scheduler.

typedef void (*irqcallback)(void *);

// run callback with the data argument just before the next context switch. It will run in the 1K handler stack!
// This function is only safe to be called either in an IRQ, or inside a kernel critical section
// (IE a task switch must not occur while it is running. It is re-entrant, however)
void irqcallback_new(irqcallback callback, void *data);

void irqcallback_run();

#endif
