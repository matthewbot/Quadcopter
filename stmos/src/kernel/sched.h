#ifndef STMOS_KERNEL_SCHED_H
#define STMOS_KERNEL_SCHED_H

#include "task.h"

void sched_add_task(struct kernel_task *task);
void sched_remove_task(struct kernel_task *task);
struct kernel_task *sched_get_current_task();

void sched_setup();
void sched_run();

#endif
