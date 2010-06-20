#include "tasklist.h"
#include "irq.h"
#include "task.h"
#include "sched.h"
#include <stm32f10x.h>
#include <stdint.h>

static struct kernel_task *tasklist_head;
volatile uint8_t tasklist_count;

static uint32_t tasklist_lastswapcycle;
static int tasklist_updatecount;
static int tasklist_irqcount;
static uint32_t tasklist_irqcurcycles;
static uint32_t tasklist_irqprevcycles;

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

	int delta = tasklist_lastswapcycle - cycle;
	if (delta <= 0)
		delta += 72000; // TODO refactor systick stuff
		
	if (task)
		task->curcycles += delta;
	else 
		tasklist_irqcurcycles += delta;
	
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
	
	tasklist_irqprevcycles = tasklist_irqcurcycles;
	tasklist_irqcurcycles = 0;
}

int tasklist_get_count() {
	return tasklist_count;
}

uint32_t tasklist_get_irq_cycles() {
	return tasklist_irqprevcycles;
}

void tasklist_irq_began() {
	if (tasklist_irqcount == 0)
		tasklist_task_swapedout(sched_get_current_task());
	
	tasklist_irqcount++;
}

void tasklist_irq_ended() {
	if (tasklist_irqcount == 0)
		return;
		
	if (tasklist_irqcount == 1)
		tasklist_task_swapedout(NULL);
	
	tasklist_irqcount--;
}

