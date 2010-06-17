#ifndef STMOS_KERNEL_MUTEX_H
#define STMOS_KERNEL_MUTEX_H

#include "task.h"
#include <stdint.h>

struct kernel_mutex {
	struct kernel_task *list_begin;
	struct kernel_task *volatile owner;
	volatile uint8_t count;
} __attribute__((packed));

void mutex_init(struct kernel_mutex *mutex);
void mutex_wait(struct kernel_mutex *mutex);
void mutex_release(struct kernel_mutex *mutex);
inline struct kernel_task *mutex_get_owner(struct kernel_mutex *mutex) { return mutex->owner; }

#endif
