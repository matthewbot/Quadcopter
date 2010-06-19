#include "tasklist.h"
#include "irq.h"
#include "task.h"
#include <stm32f10x.h>
#include <stdint.h>

static struct kernel_task *tasklist_head;
volatile uint8_t tasklist_count;

static uint32_t tasklist_lastswapcycle;
static int tasklist_updatecount;

void tasklist_task_created(struct kernel_task *task) {
	irq_disable_switch();
	
	task->tasklist_next = tasklist_head;
	task->tasklist_prev = NULL;
	if (tasklist_head)
		tasklist_head->tasklist_prev = task;
	tasklist_head = task;
	tasklist_count++;
	
	irq_enable_switch();
}

void tasklist_task_freed(struct kernel_task *task) {
	irq_disable_switch();
	
	if (task->tasklist_prev)
		task->tasklist_prev->tasklist_next = task->tasklist_next;
	else
		tasklist_head = task->tasklist_next;
	task->tasklist_next->tasklist_prev = task->tasklist_prev;
	tasklist_count--;
	
	irq_enable_switch();
}

void tasklist_task_swapedout(struct kernel_task *task) {
	uint32_t cycle = SysTick->VAL;

	if (task) {
		int delta = tasklist_lastswapcycle - cycle;
		if (delta <= 0)
			delta += 72000; // TODO refactor systick stuff
		task->curcycles += delta;
	}
	
	tasklist_lastswapcycle = cycle;
}

void tasklist_update() {
	if (++tasklist_updatecount < 100) // update every 100 ms
		return;
	tasklist_updatecount = 0;
	
	struct kernel_task *curtask = tasklist_head;
	
	while (curtask) {
		curtask->prevcycles = curtask->curcycles;
		curtask->curcycles = 0;
		curtask = curtask->tasklist_next;
	}
}

int tasklist_get_count() {
	return tasklist_count;
}

