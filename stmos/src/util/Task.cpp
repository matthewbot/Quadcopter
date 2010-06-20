#include "Task.h"

extern "C" {
	#include <stmos/kernel/task.h>
	#include <stmos/kernel/sched.h>
	#include <stmos/kernel/tick.h>
	#include <stmos/kernel/irq.h>
	#include <stmos/kernel/tasklist.h>
	#include <stmos/crt/panic.h>
}

using namespace stmos;
static void taskfunc(void *callback);

Task Task::maintask;

Task::Task(const char *name, uint8_t pri, Callback &callback, size_t stacksize) 
: ktask(task_new(name, pri, taskfunc, &callback, stacksize)) {
	if (ktask == NULL)
		panic("Out of memory to allocate task");
	ktask->userdata = this;
}

// private constructor, only used to create the hidden Task object representing the main task
Task::Task() : ktask(sched_get_current_task()) { // main task always runs global constructors
	ktask->userdata = this;
}

Task::~Task() {
	stop();
}

__attribute__((naked))
static void taskfunc(void *callback_v) {
	Callback *callback = (Callback *)callback_v;
	callback->call();
	Task::getCurrentTask()->stop();
	while (true) { }
}

void Task::suspend() {
	assert(ktask);
	assert(ktask->state == TASK_STATE_SCHEDULED);
		
	sched_remove_task(ktask);
}

void Task::resume() {
	assert(ktask);
	assert(ktask->state == TASK_STATE_NONE);
		
	sched_add_task(ktask);
}

void Task::stop() {
	assert(ktask);
	
	irq_disable_switch();
	if (ktask->state == TASK_STATE_SCHEDULED)
		sched_remove_task(ktask);
	else if (ktask->state == TASK_STATE_SLEEP)
		tick_unsleep(ktask);
		
	// even if we're freeing ourselves, we won't actually get deleted until the 
	// idle task runs (which won't happen until after we enable switching)
	task_free(ktask);

	if (sched_get_current_task() == ktask) // if we just freed and descheduled the current task
		irq_force_switch(); // we will disappear at the next context switch, so to be consistent force a switch
	
	ktask = NULL; // otherwise, clear ktask
	irq_enable_switch();
}

float Task::getCPUUsage() {
	assert(ktask);
	
	return ktask->prevcycles / 7200000.0; // TODO abstract me (SysTick period * 100ms)
}

float Task::getIRQCPUUsage() {
	return tasklist_get_irq_cycles() / 7200000.0;
}

void Task::sleep(unsigned long msecs) {
	tick_t waketick = tick_getcount() + msecs;
	struct kernel_task *curtask = sched_get_current_task();

	do {
		tick_sleep(curtask, waketick);
	} while (tick_getcount() < waketick); // loop is just in case something else wakes us up (suspend then resume) before we should
}

Task *Task::getCurrentTask() {
	return (Task *)sched_get_current_task()->userdata;
}

unsigned long Task::getCurrentTick() {
	return tick_getcount();
}

