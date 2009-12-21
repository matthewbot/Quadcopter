#ifndef STMOS_KERNEL_NOTIFIER_H
#define STMOS_KERNEL_NOTIFIER_H

#include "task.h"
#include <stdint.h>

struct kernel_notifier {
	struct kernel_tasklist_node node;
	struct kernel_task *node_end;
};

void notifier_init(struct kernel_notifier *notifier);
void notifier_wait(struct kernel_notifier *notifier);
void notifier_notify(struct kernel_notifier *notifier);
void notifier_notify_all(struct kernel_notifier *notifier);

#endif
