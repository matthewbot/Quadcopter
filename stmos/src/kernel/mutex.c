#include "mutex.h"
#include "atomic.h"
#include "sched.h"
#include "irq.h"
#include <string.h>

void mutex_init(struct kernel_mutex *mutex) {
	memset(mutex, '\0', sizeof(struct kernel_mutex));
	mutex->node_end = (struct kernel_task *)&mutex->node;
}

void mutex_wait(struct kernel_mutex *mutex) {
	struct kernel_task *curtask = sched_get_current_task();
	
	if (mutex->owner != curtask) {
		// attempt to change mutex owner from NULL to ourselves
		while (!cmpxchg((volatile uint32_t *)&mutex->owner, (uint32_t)NULL, (uint32_t)curtask)) { 
			// if it wasn't NULL,
			irq_disable_switch(); 
		
			sched_remove_task(curtask); // unschedule ourselves
			task_list_add(mutex->node_end, curtask); // and insert us at the end of the mutex's waiting queue
			mutex->node_end = curtask;
		
			irq_force_switch(); // and force a context switch
			irq_enable_switch();
		}
	}
	
	mutex->count++;
}

void mutex_release(struct kernel_mutex *mutex) {
	if (--mutex->count > 0) 
		return;
		
	irq_disable_switch();
	
	struct kernel_task *fronttask = mutex->node.next; // add the front task of the wait queue to the scheduler
	if (fronttask != NULL) {
		task_list_remove(fronttask);
		sched_add_task(fronttask);
	
		if (fronttask == mutex->node_end) // if the front task is also the end
			mutex->node_end = (struct kernel_task *)&mutex->node; // reset the end pointer to our starting node
	}
	
	mutex->owner = NULL;
	irq_enable_switch();
}

