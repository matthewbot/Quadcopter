#include "Task.h"

extern "C" {
	#include <stmos/kernel/task.h>
	#include <stmos/kernel/sched.h>
	#include <stmos/kernel/tick.h>
	#include <stmos/kernel/irq.h>
	#include <stmos/crt/panic.h>
}

using namespace stmos;
static void taskfunc(void *callback);

Task Task::maintask;

Task::Task(const char *name, uint8_t pri, Callback &callback, size_t stacksize) 
: ktask(task_new(name, pri, taskfunc, stacksize, &callback)) {
	if (ktask == NULL)
		panic("Out of memory to allocate task");
	ktask->userdata = this;
}

Task::Task() : ktask(NULL) { }

Task::~Task() {
	stop();
	task_free(ktask);
	ktask = NULL;
}

static void taskfunc(void *callback_v) {
	Callback *callback = (Callback *)callback_v;
	callback->call();
}

void Task::suspend() {
	if (!ktask)
		return;
		
	irq_disable_switch();
	wakeup();
	
	if (ktask->state == TASK_STATE_SCHEDULED)
		sched_remove_task(ktask);
	irq_enable_switch();
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

void Task::sleep(unsigned long msecs) {
	tick_t waketick = tick_getcount() + msecs;
	struct kernel_task *curtask = sched_get_current_task();

	do {
		tick_sleep(curtask, waketick);
	} while (tick_getcount() < waketick); // loop is just in case something else wakes us up (suspend then resume) before we should
}

Task *Task::getCurrentTask() {
	struct kernel_task *ktask = sched_get_current_task();
	Task *task = (Task *)ktask->userdata;
	
	if (task)
		return task;
		
	maintask.ktask = ktask;
	ktask->userdata = &maintask;
	return &maintask;
}

void Task::wakeup() {
	if (ktask->state == TASK_STATE_SLEEP)
		tick_unsleep(ktask);
}

unsigned long Task::getCurrentTick() {
	return tick_getcount();
}
