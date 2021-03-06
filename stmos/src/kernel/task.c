#include "task.h"
#include "irq.h"
#include "tasklist.h"
#include <stm32f10x.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TASK_MINSTACK 16*4

struct kernel_task *freetasks;

struct kernel_task *task_new(const char *name, kernel_taskpri pri, kernel_taskfunc func, void *data, size_t stacksize) {
	assert(stacksize >= TASK_MINSTACK);
	size_t size = sizeof(struct kernel_task) + stacksize; // calculate memory size
	char *mem = malloc(size); // allocate memory
	
	if (mem == NULL)
		return NULL;
	
	struct kernel_task *task = task_new_inplace(name, pri, func, data, mem, size);
	task->needfree = true;
	return task;
}

struct kernel_task *task_new_inplace(const char *name, kernel_taskpri pri, kernel_taskfunc func, void *data, char *buf, size_t bufsize) {
	struct kernel_task *task = (struct kernel_task *)buf;
	
	char *rawsp = (buf + bufsize); // calculate top of stack
	uint32_t *sp = (uint32_t *)(rawsp - ((int)rawsp % 4)); // align it
	*--sp = 0x01000000; // saved PSR, T bit must be set
	*--sp = (uint32_t)func; // PC
	sp -= 5; // Skip LR, R12, R3, R2 and R1
	*--sp = (uint32_t)data; // R0
	sp -= 8; // R11 - R4
	
	memset(task, '\0', sizeof(struct kernel_task));
	task->sp = sp;
	task->pri = pri;
	strncpy(task->name, name, sizeof(task->name));
	task->stackguard = TASK_STACKGUARD_VALUE;
	tasklist_task_created(task);
	
	return task;
}

void task_list_add(struct kernel_task *listpos, struct kernel_task *task) {
	assert(listpos != NULL);
	assert(task != NULL);
	task_assertstack(task);
	
	struct kernel_task *next = listpos->list_next;
	listpos->list_next = task;
	task->list_prev = listpos;
	task->list_next = next;
	if (next != NULL)
		next->list_prev = task;
}

struct kernel_task *task_list_remove(struct kernel_task *task) {
	assert(task != NULL);
	task_assertstack(task);
	
	struct kernel_task *next = task->list_next; // save next and prev
	struct kernel_task *prev = task->list_prev;
	
	task->list_prev = task->list_next = NULL; // clear the current task's pointers
	
	prev->list_next = next; // update next and prev's pointers
	if (next != NULL)
		next->list_prev = prev;

	return next;
}

void task_list_append_sorted(struct kernel_task *list, struct kernel_task *task) {
	assert(list != NULL);
	assert(task != NULL);
	task_assertstack(task);

	const uint8_t ourpri = task->pri;
	while (list->list_next && list->list_next->pri <= ourpri)
		list = list->list_next;
		
	task_list_add(list, task);
}

void task_free(struct kernel_task *task) {
	assert(task != NULL);
	if (!task->needfree)
		return;
		
	task_assertstack(task);
	irq_disable_switch();
	task->list_next = freetasks;
	freetasks = task;
	irq_enable_switch();
}

void task_gc() {
	while (true) {
		irq_disable_switch();
	
		struct kernel_task *task = freetasks;
		if (task == NULL)
			break;
		freetasks = task->list_next;
		
		irq_enable_switch(); // the next two commands use mutexes, so context switches must be enabled
		tasklist_task_freed(task);
		free(task);
	}
	
	irq_enable_switch();
}

