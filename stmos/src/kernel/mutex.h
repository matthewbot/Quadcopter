#ifndef STMOS_KERNEL_MUTEX_H
#define STMOS_KERNEL_MUTEX_H

#include "task.h"
#include <stdint.h>

struct kernel_mutex {
	struct kernel_task *list_begin;
	struct kernel_task *list_end;
	volatile struct kernel_task *owner;
	volatile uint32_t count;
};

void mutex_init(struct kernel_mutex *mutex);
void mutex_wait(struct kernel_mutex *mutex);
void mutex_release(struct kernel_mutex *mutex);

#endif
