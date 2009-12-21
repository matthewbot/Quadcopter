#include "notifier.h"
#include "sched.h"
#include "task.h"
#include "irq.h"

void notifier_init(struct kernel_notifier *notifier) {
	notifier->node.next = NULL;
	notifier->node_end = (struct kernel_task *)&notifier->node;
}

void notifier_wait(struct kernel_notifier *notifier) {
	struct kernel_task *curtask = sched_get_current_task();
	
	irq_disable_switch();
	
	sched_remove_task(curtask);
	task_list_add(notifier->node_end, curtask);
	notifier->node_end = curtask;
	
	irq_force_switch();
	irq_enable_switch();
}

void notifier_notify(struct kernel_notifier *notifier) {
	irq_disable_switch();
	
	struct kernel_task *fronttask = notifier->node.next;
	if (fronttask != NULL) {
		task_list_remove(fronttask);
		sched_add_task(fronttask);
		
		if (fronttask == notifier->node_end)
			notifier->node_end = (struct kernel_task *)&notifier->node;
	}
	
	irq_enable_switch();
}

void notifier_notify_all(struct kernel_notifier *notifier) {
	irq_disable_switch();
	
	struct kernel_task *cur = notifier->node.next;
	while (cur != NULL) {
		struct kernel_task *next = cur->listnode.next;
		sched_add_task(cur);
		cur = next;
	}
	notifier->node.next = NULL;
	notifier->node_end = (struct kernel_task *)&notifier->node;
	
	irq_enable_switch();
}
