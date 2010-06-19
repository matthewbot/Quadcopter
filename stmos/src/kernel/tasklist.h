#ifndef STMOS_KERNEL_TASKLIST_H
#define STMOS_KERNEL_TASKLIST_H

struct kernel_task;

void tasklist_task_created(struct kernel_task *task);
void tasklist_task_freed(struct kernel_task *task);
void tasklist_task_swapedout(struct kernel_task *task);
void tasklist_update();

int tasklist_get_count();


#endif
