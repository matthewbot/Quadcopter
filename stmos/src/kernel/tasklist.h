#ifndef STMOS_KERNEL_TASKLIST_H
#define STMOS_KERNEL_TASKLIST_H

#include <stdint.h>

struct kernel_task;

// hooks called by parts of the OS
void tasklist_task_created(struct kernel_task *task);
void tasklist_task_freed(struct kernel_task *task);
void tasklist_task_swapedout(struct kernel_task *task);
void tasklist_irq_began();
void tasklist_irq_ended();
void tasklist_update();

int tasklist_get_count();
uint32_t tasklist_get_irq_cycles();


#endif
