#ifndef STMOS_KERNEL_MUTEX_H
#define STMOS_KERNEL_MUTEX_H

#include "task.h"
#include <stdint.h>

struct kernel_mutex {
	struct kernel_task *list_begin;
	volatile struct kernel_task *owner;
	volatile uint8_t count;
} __attribute__((packed));

void mutex_init(struct kernel_mutex *mutex);
void mutex_wait(struct kernel_mutex *mutex);
void mutex_release(struct kernel_mutex *mutex);

#endif
