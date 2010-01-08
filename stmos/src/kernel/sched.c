#include "sched.h"
#include "irq.h"
#include <stmos/util/cexts.h>
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>
#include <stdbool.h>

// not static so some assembly code can poke it
struct kernel_task *sched_curtask;
static struct kernel_task *nexttask; // when the next task can't be determined from the current task

static struct kernel_task *schedlist;
static kernel_taskpri schedlist_maxpri=0xFF;

struct kernel_task *sched_get_current_task() { return sched_curtask; }

void sched_add_task(struct kernel_task *task) {
	irq_disable_switch();
	
	if (task->pri < schedlist_maxpri) {
		schedlist_maxpri = task->pri;
		if (sched_curtask != NULL)
			irq_force_switch();
	}
	
	struct kernel_task *insertpoint = (struct kernel_task *)&schedlist;
	while (insertpoint->list_next != NULL && task->pri > insertpoint->list_next->pri) {
		insertpoint = insertpoint->list_next;
	}
			
	task_list_add(insertpoint, task);
	task->state = TASK_STATE_SCHEDULED;
	
	irq_enable_switch();
}

void sched_remove_task(struct kernel_task *task) {
	irq_disable_switch();
	
	// if we're unscheduling the current task, or the saved next task
	if (task == sched_curtask || UNLIKELY(task == nexttask))
		nexttask = task->list_next; // we need to save the next task
			
	task_list_remove(task);
	task->state = TASK_STATE_NONE;
	
	if (UNLIKELY(task->pri == schedlist_maxpri)) 
		schedlist_maxpri = schedlist->pri;
		
	irq_enable_switch();
}

void sched_setup() {
	sched_curtask = schedlist;
}

void sched_run() {
	task_assertstack(sched_curtask);

	// first, we need to determine the next task
	if (UNLIKELY(nexttask != NULL)) { // if the next task has been saved (because the current task no longer is linked in the list)
		sched_curtask = nexttask; // use it as the current task
		nexttask = NULL; // clear nexttask flag
	} else { // no saved next task
		sched_curtask = sched_curtask->list_next; // so use the next task in the linked list
		if (UNLIKELY(sched_curtask == NULL)) // if we're at the end of the linked list
			goto schedfirst; // we need to schedule the first task
	}
	
	if (UNLIKELY(sched_curtask->pri > schedlist_maxpri)) // if our new task is actually a lower priority than the current max
		goto schedfirst; // we need to schedule from the top again
		
	return; // otherwise we're done!
schedfirst:
	sched_curtask = schedlist;
}

