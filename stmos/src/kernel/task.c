#include "task.h"
#include "irq.h"
#include <stm32f10x.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TASK_MINSTACK 16*4

struct kernel_tasklist_node freetasks;

struct kernel_task *task_new(const char *name, kernel_taskpri pri, kernel_taskfunc func, size_t stacksize, void *data) {
	stacksize += TASK_MINSTACK;
	size_t size = sizeof(struct kernel_task) + stacksize; // calculate memory size
	struct kernel_task *task = malloc(size); // allocate memory
	
	if (task == NULL)
		return NULL;
	
	char *rawsp = ((char *)task + size); // calculate top of stack
	uint32_t *sp = (uint32_t *)(rawsp - ((int)rawsp % 4)); // align it
	*--sp = 0x01000000; // saved PSR, T bit must be set
	*--sp = (uint32_t)func; // PC
	sp -= 5; // Skip LR, R12, R3, R2 and R1
	*--sp = (uint32_t)data; // R0
	sp -= 8; // R11 - R4
	
	task->sp = sp;
	task->pri = pri;
	strncpy(task->name, name, sizeof(task->name));
	task->listnode.prev = task->listnode.next = NULL;
	task->userdata = NULL;
	task->state = TASK_STATE_NONE;
	task->stackguard = TASK_STACKGUARD_VALUE;
	return task;
}

void task_list_add(struct kernel_task *listpos, struct kernel_task *task) {
	struct kernel_task *next = listpos->listnode.next;
	listpos->listnode.next = task;
	task->listnode.prev = listpos;
	task->listnode.next = next;
	if (next != NULL)
		next->listnode.prev = task;
}

struct kernel_task *task_list_remove(struct kernel_task *task) {
	struct kernel_task *next = task->listnode.next; // save next and prev
	struct kernel_task *prev = task->listnode.prev;
	
	task->listnode.prev = task->listnode.next = NULL; // clear the current task's pointers
	
	prev->listnode.next = next; // update next and prev's pointers
	if (next != NULL)
		next->listnode.prev = prev;

	return next;
}

void task_free(struct kernel_task *task) {
	irq_disable_switch();
	task->listnode.next = freetasks.next;
	freetasks.next = task;
	irq_enable_switch();
}

void task_gc() {
	while (true) {
		irq_disable_switch();
	
		struct kernel_task *task = freetasks.next;
		if (task == NULL)
			break;
		freetasks.next = task->listnode.next;
		
		free(task);
		
		irq_enable_switch(); // in between every tasks toggle interrupts so a context switch can interrupt a GC cycle
	}
	
	irq_enable_switch();
}
