#ifndef STMOS_KERNEL_NOTIFIER_H
#define STMOS_KERNEL_NOTIFIER_H

#include "task.h"
#include <stdint.h>

struct kernel_notifier {
	struct kernel_task *list_start;
	struct kernel_task *list_end;
};

struct kernel_mutex;

void notifier_init(struct kernel_notifier *notifier);
void notifier_wait(struct kernel_notifier *notifier);
void notifier_wait_release(struct kernel_notifier *notifier, struct kernel_mutex *mutex);
void notifier_notify(struct kernel_notifier *notifier);
void notifier_notify_all(struct kernel_notifier *notifier);

#endif
