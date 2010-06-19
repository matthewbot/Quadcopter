#include "tasklist.h"
#include "mutex.h"
#include "task.h"
#include <stdint.h>

static struct kernel_mutex tasklist_mutex;
static struct kernel_task *tasklist_head;
volatile uint8_t tasklist_count;

void tasklist_init() {
	mutex_init(&tasklist_mutex);
}

void tasklist_task_created(struct kernel_task *task) {
	mutex_wait(&tasklist_mutex);
	
	task->tasklist_next = tasklist_head;
	task->tasklist_prev = NULL;
	if (tasklist_head)
		tasklist_head->tasklist_prev = task;
	tasklist_head = task;
	tasklist_count++;
	
	mutex_release(&tasklist_mutex);
}

void tasklist_task_freed(struct kernel_task *task) {
	mutex_wait(&tasklist_mutex);
	
	if (task->tasklist_prev)
		task->tasklist_prev->tasklist_next = task->tasklist_next;
	else
		tasklist_head = task->tasklist_next;
	task->tasklist_next->tasklist_prev = task->tasklist_prev;
	tasklist_count--;
	
	mutex_release(&tasklist_mutex);
}

int tasklist_get_count() {
	return tasklist_count;
}

