#include "notifier.h"
#include "mutex.h"
#include "sched.h"
#include "task.h"
#include "irq.h"

void notifier_init(struct kernel_notifier *notifier) {
	notifier->list_start = NULL;
	notifier->list_end = (struct kernel_task *)&notifier->list_start;
}

void notifier_wait(struct kernel_notifier *notifier) {
	struct kernel_task *curtask = sched_get_current_task();
	
	irq_disable_switch();
	
	sched_remove_task(curtask);
	task_list_add(notifier->list_end, curtask);
	notifier->list_end = curtask;
	
	irq_force_switch();
	irq_enable_switch();
}

void notifier_wait_release(struct kernel_notifier *notifier, struct kernel_mutex *mutex) {
	irq_disable_switch();
	notifier_wait(notifier);
	mutex_release(mutex);
	irq_enable_switch();
	
	mutex_wait(mutex);
}

void notifier_wait_leave(struct kernel_notifier *notifier, int irq) {
	notifier_wait(notifier);
	irq_leave_critical(irq);
	
	irq_enter_critical(irq);
}

void notifier_wait_leave_release(struct kernel_notifier *notifier, int irq, struct kernel_mutex *mutex) {
	notifier_wait(notifier);
	mutex_release(mutex); // must release mutex first, we're using the critical section to prevent context switches
	irq_leave_critical(irq);

	mutex_wait(mutex);
	irq_enter_critical(irq);
}

void notifier_notify(struct kernel_notifier *notifier) {
	irq_disable_switch();
	
	struct kernel_task *fronttask = notifier->list_start;
	if (fronttask != NULL) {
		task_list_remove(fronttask);
		sched_add_task(fronttask);
		
		if (fronttask == notifier->list_end)
			notifier->list_end = (struct kernel_task *)&notifier->list_start;
	}
	
	irq_enable_switch();
}

void notifier_notify_all(struct kernel_notifier *notifier) {
	irq_disable_switch();
	
	struct kernel_task *cur = notifier->list_start;
	while (cur != NULL) {
		struct kernel_task *next = cur->list_next;
		sched_add_task(cur);
		cur = next;
	}
	notifier->list_start = NULL;
	notifier->list_end = (struct kernel_task *)&notifier->list_start;
	
	irq_enable_switch();
}
