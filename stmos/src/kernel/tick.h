#ifndef STMOS_KERNEL_TICK_H
#define STMOS_KERNEL_TICK_H

#include "task.h"
#include <stdint.h>

typedef uint32_t tick_t;

tick_t tick_getcount();
void tick_sleep(struct kernel_task *task, tick_t waketick);
void tick_unsleep(struct kernel_task *task);
void tick_wake(struct kernel_task *task);

void tick_run();

#endif
