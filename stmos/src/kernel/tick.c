#include "tick.h"
#include "sched.h"
#include "irq.h"

static tick_t curtick;
static struct kernel_task *ticklist;

tick_t tick_getcount() {
	return curtick;
}

void tick_sleep(struct kernel_task *task, tick_t waketick) {
	irq_disable_switch();
	
	sched_remove_task(task);
	task->list_data.num = waketick;
	
	struct kernel_task *insertpoint = (struct kernel_task *)&ticklist;
	while (insertpoint->list_next != NULL && waketick > insertpoint->list_next->list_data.num) {
		insertpoint = insertpoint->list_next;
	}
	
	task_list_add(insertpoint, task);
	task->state = TASK_STATE_SLEEP;
	
	if (sched_get_current_task() == task)
		irq_force_switch();
		
	irq_enable_switch();
}

void tick_unsleep(struct kernel_task *task) {
	irq_disable_switch();
	
	task_list_remove(task);
	task->state = TASK_STATE_NONE;
	
	irq_enable_switch();
}

void tick_wake(struct kernel_task *task) {
	irq_disable_switch();

	task_list_remove(task);
	sched_add_task(task);
	
	irq_enable_switch();
}

void tick_run() {
	curtick++;
	
	struct kernel_task *curtask = ticklist;
	while (curtask != NULL) {
		if (curtick >= curtask->list_data.num) {
			struct kernel_task *nexttask = task_list_remove(curtask);
			sched_add_task(curtask);
			curtask = nexttask;
		} else {
			break; // don't carry on, list is sorted so all entries after this one are in the future
		}
	}
}

