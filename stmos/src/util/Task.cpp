#include "Task.h"

extern "C" {
	#include <stmos/kernel/task.h>
	#include <stmos/kernel/sched.h>
	#include <stmos/kernel/tick.h>
	#include <stmos/kernel/irq.h>
}

using namespace stmos;
static void taskfunc(void *callback);

Task::Task(const char *name, uint8_t pri, Callback &callback, size_t stacksize) 
: ktask(task_new(name, pri, taskfunc, stacksize, &callback)) {
	sched_add_task(ktask);
}

static void taskfunc(void *callback_v) {
	Callback *callback = (Callback *)callback_v;
	callback->call();
}

void Task::suspend() {
	if (!ktask)
		return;
		
	wakeup();
	
	if (ktask->state == TASK_STATE_SCHEDULED)
		sched_remove_task(ktask);
}

void Task::resume() {
	if (!ktask)
		return;
		
	if (ktask->state == TASK_STATE_NONE)
		sched_add_task(ktask);
}

void Task::stop() {
	if (!ktask)
		return;
	
	wakeup(); 
	
	irq_disable_switch();
	if (ktask->state == TASK_STATE_SCHEDULED)
		sched_remove_task(ktask);
		
	// even if we're freeing ourselves, we won't actually get deleted until the 
	// idle task runs (which won't happen until after we enable switching)
	task_free(ktask);

	if (sched_get_current_task() == ktask) // if we just freed and descheduled the current task
		irq_force_switch(); // we will disappear at the next context switch, so to be consistent force a switch
	
	ktask = NULL; // otherwise, clear ktask
	irq_enable_switch();
}

void Task::sleep(long msecs) {
	tick_t waketick = tick_getcount() + msecs;
	struct kernel_task *curtask = sched_get_current_task();

	do {
		tick_sleep(curtask, waketick);
	} while (tick_getcount() != waketick); // loop is just in case something else wakes us up (suspend then resume) before we should
}

void Task::wakeup() {
	if (ktask->state == TASK_STATE_SLEEP)
		tick_unsleep(ktask);
}

